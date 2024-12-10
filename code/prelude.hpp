#define length(array) (sizeof(array) / sizeof((array)[0]))

typedef int                I32;
typedef long long          I64;
typedef unsigned char      U8;
typedef unsigned int       U32;
typedef unsigned long long U64;
typedef float              F32;

template <typename A>
static A min(A a, A b) {
  return a < b ? a : b;
}

static bool is_digit(U8 c) {
  return '0' <= c && c <= '9';
}

struct String {
  U8* data = NULL;
  I64 size = 0;

  String() {
  }

  String(const char* string) {
    data = (U8*) string;
    size = string == NULL ? 0 : strlen(string);
  }

  String(U8* data, I64 size) {
    this->data = data;
    this->size = size;
  }

  U8& operator[](I64 index) {
    return data[index];
  }
};

static bool operator==(String a, String b) {
  return a.size == b.size && memcmp(a.data, b.data, a.size) == 0;
}

static String prefix(String a, I64 count) {
  a.size = count;
  return a;
}

static String suffix(String a, I64 count) {
  a.data += count;
  a.size -= count;
  return a;
}

static String to_string(I64 n, U8 storage[20]) {
  U8*  end      = &storage[20];
  U8*  start    = end;
  bool negative = false;
  if (n < 0) {
    negative = true;
    n        = -n;
  }
  do {
    start  = start - 1;
    *start = n % 10 + '0';
    n      = n / 10;
  } while (n > 0);
  if (negative) {
    start  = start - 1;
    *start = '-';
  }
  return String(start, end - start);
}
