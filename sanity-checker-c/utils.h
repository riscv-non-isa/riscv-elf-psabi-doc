// These macros are for compile-time sanity checks    -*- C -*-

// Check sizeof(type) == size, _Alignof(type) == align
#define CHECK_SIZE_ALIGN(type, size, align)                                    \
  CHECK_EQ("sizeof(" #type ")", size, sizeof(type));                           \
  CHECK_EQ("_Alignof(" #type ")", align, _Alignof(type));

#define CHECK_DEFINED_EQ(name, value)                                          \
  CHECK_EQ(#name " (preprocessor define)", value, name);

// Check expected == actual, including expr_str in error message
#define CHECK_EQ(expr_str, expected, actual)                                   \
  _Static_assert((expected) == (actual), expr_str " should be " #expected);
