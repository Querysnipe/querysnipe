#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "prelude.hpp"
#include "print.hpp"
#include "time.hpp"

static U64 cpu_strptime;
static U64 cpu_parse_time;

static U64 get_cpu_time() {
  return clock_gettime_nsec_np(CLOCK_PROCESS_CPUTIME_ID);
}

static void run_test(Time time_offset, String input) {
  println(INFO "input=\"", input, "\"");
  flush();

  U64 start = get_cpu_time();
  
  struct tm date = {};
  assert(strptime((char*) input.data, "%F", &date) != NULL);

  /*
    println(
    INFO
    "time.tm_year=" , (I64) time.tm_year,
    " time.tm_mon=" , (I64) time.tm_mon,
    " time.tm_mday=", (I64) time.tm_mday
    );
  */

  time_t expected = mktime(&date);
  assert(expected != -1);

  cpu_strptime += get_cpu_time() - start;

  start = get_cpu_time();
  
  Time parsed = 0;
  assert(parse_time(time_offset, input, &parsed));

  cpu_parse_time += get_cpu_time() - start;

  println(INFO "parsed=", (I64) parsed, " expected=", (I64) expected);

  flush();
  assert(parsed == expected);
}

int main() {
  atexit(flush);

  assert(find_timestamp("2024-10-09T00:00:00") == "2024-10-09");
  assert(find_timestamp("{\"level\":\"INFO\",\"time\":\"2024-10-09T00:00:00") == "2024-10-09");

  Time time_offset = get_timezone_offset();
  char storage[]   = "2024-10-09T00:00:00";

  for (I64 i = 0; i < 1000; i++) {
    I64 year  = rand() % (2038 - 1970) + 1970;
    I64 month = rand() % 12 + 1;
    I64 day   = rand() % get_days_in_month(year, month) + 1;
    
    String input = storage;
    input[0]     = (year  / 1000) % 10 + '0';
    input[1]     = (year  /  100) % 10 + '0';
    input[2]     = (year  /   10) % 10 + '0';
    input[3]     = (year  /    1) % 10 + '0';
    input[5]     = (month /   10) % 10 + '0';
    input[6]     = (month /    1) % 10 + '0';
    input[8]     = (day   /   10) % 10 + '0';
    input[9]     = (day   /    1) % 10 + '0';
    
    run_test(time_offset, input);
  }

  println(INFO "cpu_strptime=", (I64) cpu_strptime, " cpu_parse_time=", (I64) cpu_parse_time);
}
