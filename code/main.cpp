#include <aio.h>
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "prelude.hpp"
#include "print.hpp"
#include "arena.hpp"
#include "builder.hpp"
#include "time.hpp"

struct File {
  I32         fd;
  struct stat info;
};

static void open_log_files(Arena* path_arena, String root, Array<File>* output) {
  I64 path_saved = save(path_arena);
  
  Builder path = make_builder(path_arena);
  push(&path, root);
  push(&path, 0);

  println(INFO "Opening ", path.result, ".");
  
  I32 fd = open((char*) path.result.data, O_RDONLY);
  if (fd == -1) {
    println(ERROR "Failed to open file: ", get_error(), '.');
    exit(EXIT_FAILURE);
  }

  struct stat info = {};
  if (fstat(fd, &info) == -1) {
    println(ERROR "Failed to stat file: ", get_error(), '.');
    exit(EXIT_FAILURE);
  }
  
  mode_t mode = info.st_mode;

  pop(&path, 1);
    
  if (S_ISREG(mode)) {
    push(output, (File) { fd, info });
  }

  if (S_ISDIR(mode)) {
    DIR* dir = fdopendir(fd);

    while (true) {
      errno                = 0;
      struct dirent* entry = readdir(dir);
      if (entry == NULL) {
	break;
      }

      String name = entry->d_name;
      if (name != "." && name != "..") {
	push(&path, '/');
	push(&path, name);
	push(&path, 0);
	open_log_files(path_arena, path.result, output);
	flush();
	pop(&path, name.size + 2);
      }
    }

    if (errno != 0) {
      println(WARN "Failed to read from directory: ", get_error(), ".");
    }
      
    if (closedir(dir) != 0) {
      println(WARN "Failed to close directory: ", get_error(), ".");
    }
  }
    
  restore(path_arena, path_saved);
}

struct Query {
  String value;
  Time   range[2];
};

static bool is_line_in_range(Time time_offset, String line, Query query) {
  String found     = find_timestamp(line);
  I64    timestamp = 0;
  bool   in_range  = false;
  if (found.size > 0 && parse_time(time_offset, found, &timestamp)) {
    Time* range = query.range;
    in_range    = in_range && range[0] == 0 || range[0] <= timestamp;
    in_range    = in_range && range[1] == 0 || range[1] >= timestamp;
  }
  return in_range;
}

static void display(Arena* arena, Query query, String line) {  
  I64     saved  = save(arena);
  Builder output = make_builder(arena);
  
  I64  word_start  = 0;
  I64  qoute_start = 0;
  bool in_qoute    = false;
  bool found       = false;
  for (I64 i = 0; i <= line.size; i++) {
    if (i == line.size || line[i] == ' ' || line[i] == '"') {
      String word = { line.data + word_start, i - word_start };
      if (word == query.value) {
	found = true;
	push(&output, "\x1b[35;1m"); /* Purple, bold highlight. */
	push(&output, word);
	push(&output, "\x1b[0m");    /* Reset color. */
      } else {
	push(&output, word);
      }
      if (i < line.size) {
	push(&output, line[i]);
      }
      word_start = i + 1;
    }
  }
  
  push(&output, '\n');
  if (found) {
    print(output.result);
  }
  restore(arena, saved);
}

static void handle_bytes(Arena* arena, Builder* line, Time time_offset, Query query, String buffer) {
  for (I64 i = 0; i < buffer.size; i++) {
    U8 c = buffer[i];
    if (c == '\n') {
      if (is_line_in_range(time_offset, line->result, query)) {
	display(arena, query, line->result);
      }
      reset(line);
    } else {
      push(line, c);
    }
  }
}

int main(int argc, char** argv) {
  atexit(flush);

  if (argc < 3) {
    println(ERROR "Expected at least two arguments.");
    println(INFO  "Usage: querysnipe PATH TERM [START] [END]");
    exit(EXIT_FAILURE);
  }

  Time time_offset = get_timezone_offset();

  String path = argv[1];

  Query query = {};
  query.value = argv[2];

  for (I64 i = 0; i < 2; i++) {
    if (argc > 3 + i) {
      if (!parse_time(time_offset, argv[3 + i], &query.range[i])) {
	println(ERROR "Failed to parse start or end time.");
	exit(EXIT_FAILURE);
      }
    }
  }

  Arena arenas[3] = {};
  for (I64 i = 0; i < length(arenas); i++) {
    arenas[i] = make_arena(1ll << 32);
  }

  Array<File> log_files = make_array<File>(&arenas[0]);
  open_log_files(&arenas[1], path, &log_files);

  String  buffer = push_bytes(&arenas[0], 2 * 2 * 1024);
  Builder line   = make_builder(&arenas[2]);

  for (I64 i = 0; i < log_files.count; i++) {
    File file = log_files[i];

    struct aiocb operations[16] = {};
    for (int i = 0; i < length(operations); i++) {
    }

    while (true) {
      I64 bytes_read = read(file.fd, buffer.data, buffer.size);
      if (bytes_read == -1) {
	println(ERROR "Failed to open file: ", get_error(), '.');
	break;
      }
      if (bytes_read == 0) {
	break;
      }

      // println(INFO "Read ", bytes_read, " bytes.");
      handle_bytes(&arenas[0], &line, time_offset, query, prefix(buffer, bytes_read));
    }

    if (close(file.fd) == -1) {
      println(WARN "Failed to close file.");
    }
  }
}
