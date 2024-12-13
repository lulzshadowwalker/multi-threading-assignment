#ifdef TEST

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define OUT

#define run(func) \
  do { \
    printf("Running " #func " ... "); \
    clock_t start = clock(); \
    func(); \
    clock_t end = clock(); \
    double duration = (double)(end - start) / CLOCKS_PER_SEC; \
    printf("(ok) [%.2f ms]\n", duration * 1000); \
  } while (0)

bool parse_int(const char *str, OUT int *res);
bool is_prime(int n);
bool is_palindrome(int n);

void test_parse_int() {
  int result;

  assert(parse_int("123", &result));
  assert(result == 123);

  assert(parse_int("0", &result));
  assert(result == 0);

  assert(parse_int("-123", &result));
  assert(result == -123);

  assert(!parse_int("123a", &result));
  assert(!parse_int("a123", &result));

  assert(!parse_int("12345678901234567890", &result));
  assert(!parse_int("-12345678901234567890", &result));
}

void test_is_prime() {
  assert(!is_prime(0));
  assert(!is_prime(1));
  assert(is_prime(2));
  assert(is_prime(3));
  assert(!is_prime(4));
  assert(is_prime(5));
  assert(!is_prime(6));
  assert(is_prime(7));
  assert(!is_prime(8));
  assert(!is_prime(9));
  assert(!is_prime(10));

  assert(is_prime(700666007));
  assert(is_prime(700666009));
  assert(is_prime(700666019));

  assert(!is_prime(700666000));
  assert(!is_prime(700666032));
  assert(is_prime(700666007));
}

void test_is_palindrome() {
  assert(is_palindrome(0));
  assert(is_palindrome(1));
  assert(is_palindrome(11));
  assert(is_palindrome(121));
  assert(is_palindrome(1221));
  assert(is_palindrome(12321));
  assert(is_palindrome(123321));
  assert(is_palindrome(700666007));

  assert(!is_palindrome(12));
  assert(!is_palindrome(123));
  assert(!is_palindrome(1234));
  assert(!is_palindrome(12345));
}

int main() {
  printf("\n");

  run(test_parse_int);

  run(test_is_prime);

  run(test_is_palindrome);

  printf("\nAll tests passed!\n");
}

#endif
