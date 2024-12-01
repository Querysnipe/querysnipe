struct Builder {
  Arena* arena;
  String result;
};

static Builder make_builder(Arena* arena) {
  Builder builder     = {};
  builder.arena       = arena;
  builder.result.data = &arena->memory[arena->used];
  return builder;
}

static void push(Builder* builder, U8 c) {
  Arena* arena = builder->arena;
  assert(arena->used < arena->size);
  arena->memory[arena->used] = c;
  arena->used++;
  builder->result.size++;
}

static void push(Builder* builder, String s) {
  copy_string(builder->arena, s);
  builder->result.size += s.size;
}

static void pop(Builder* builder, I64 count) {
  builder->arena->used -= count;
  builder->result.size -= count;
}

static void reset(Builder* builder) {
  builder->arena->used -= builder->result.size;
  builder->result.size = 0;
}
