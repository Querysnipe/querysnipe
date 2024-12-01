struct Arena {
  U8* memory;
  I64 used;
  I64 size;
};

static Arena make_arena(I64 size) {
  U8* memory = (U8*) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
  assert(memory != MAP_FAILED);

  Arena arena  = {};
  arena.memory = memory;
  arena.size   = size;
  return arena;
}

static I64 align(I64 address, I64 alignment) {
  return (address + alignment - 1) & ~(alignment - 1);
}

template <typename T>
static T* end(Arena* arena) {
  arena->used = align(arena->used, alignof(T));
  return (T*) &arena->memory[arena->used];
}

static String push_bytes(Arena* arena, I64 size) {
  String result = { (U8*) &arena->memory[arena->used], size };
  memset(result.data, 0, result.size);

  assert(arena->used < arena->size);
  arena->used += size;
  return result;
}

template <typename T>
static T* push(Arena* arena) {
  arena->used = align(arena->used, alignof(T));
  return (T*) push_bytes(arena, sizeof(T)).data;
}

template <typename T>
static T* push_array(Arena* arena, I64 count) {
  arena->used = align(arena->used, alignof(T));
  return (T*) push_bytes(arena, sizeof(T) * count).data;
}

static I64 save(Arena* arena) {
  return arena->used;
}

static void restore(Arena* arena, I64 saved) {
  arena->used = saved;
}

static String copy_string(Arena* arena, String s) {
  String result = push_bytes(arena, s.size);
  memcpy(result.data, s.data, result.size);
  return result;
}

template <typename T>
struct Array {
  Arena* arena;
  T*     elements;
  I64    count;

  T& operator[](I64 index) {
    return elements[index];
  }
};

template <typename T>
static Array<T> make_array(Arena* arena) {
  Array<T> array = {};
  array.arena    = arena;
  array.elements = end<T>(arena);
  return array;
}

template <typename T>
static void push(Array<T>* array, T element) {
  T* storage = push<T>(array->arena);
  *storage   = element;
  array->count++;
}

template <typename T>
static T pop(Array<T>* array) {
  T element = array->elements[array->count - 1];
  array->count--;
  return element;
}
