#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define OUT

bool parse_int(const char *str, OUT int *res) {
  char *endptr;
  errno = 0;

  long number = strtol(str, &endptr, 10);

  if (errno != 0 || *endptr != '\0' || number < INT_MIN || number > INT_MAX) {
    return false;
  }

  *res = (int)number;
  return true;
}

bool is_prime(int n) {
  if (n < 2) {
    return false;
  }

  for (int i = 2; i * i <= n; i++) {
    if (n % i == 0) {
      return false;
    }
  }

  return true;
}

bool is_palindrome(int n) {
  if (n < 0)
    return false;

  int original = n;
  int reversed = 0;

  while (n != 0) {
    int digit = n % 10;
    reversed = reversed * 10 + digit;
    n /= 10;
  }

  return original == reversed;
}

#ifndef TEST

unsigned int prime_count;
unsigned int palindrome_count;
unsigned int palindromic_prime_count;
unsigned int total_count;

int main(int argc, const char *argv[]) {
  //  NOTE: Check the number of arguments and print usage if needed
  if (argc < 2) {
    printf("Usage: assignment <number-of-threads>\n");
    return 1;
  }

  //  NOTE: Read and parse the number of threads from the command line
  int thread_count;
  if (!parse_int(argv[1], &thread_count) || thread_count < 1) {
    printf("Invalid number of threads: %s\n", argv[1]);
    return 1;
  }

  //  NOTE: Read the input range from the file
  const char *filepath = "in.txt";
  FILE *input = fopen(filepath, "r");
  if (input == NULL) {
    printf("Cannot open file: %s\n", filepath);
    return 1;
  }

  unsigned int range_start, range_end;
  if (fscanf(input, "%d %d", &range_start, &range_end) != 2) {
    printf("Invalid input file format\n");
    fclose(input);
    return 1;
  }
  fclose(input);

  for (int i = range_start; i <= range_end; ++i) {
    total_count++;

    bool _is_prime = false;
    bool _is_palindrome = false;

    if (is_prime(i)) {
      _is_prime = true;
      prime_count++;
    }

    if (is_palindrome(i)) {
      _is_palindrome = true;
      palindrome_count++;
    }

    if (_is_prime && _is_palindrome) {
      palindromic_prime_count++;
    }
  }

  printf("totalNums=%d, numOfPrime=%d, numOfPalindrome=%d, "
         "numOfPalindromicPrime=%d\n",
         total_count, prime_count, palindrome_count, palindromic_prime_count);

  return 0;
}

#endif
