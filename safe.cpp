#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#define OUT

bool parse_int(const char *str, OUT int *res);
bool is_prime(int n);
bool is_palindrome(int n);

// handles the range of numbers assigned to the thread and counts the number of
// primes, palindromes, and palindromic primes
void *handle(void *arg);

#ifdef SAFE
#ifndef TEST

std::vector<int> primes;
std::vector<int> palindromes;
std::vector<int> palindromic_primes;

unsigned int prime_count;
unsigned int palindrome_count;
unsigned int palindromic_prime_count;
unsigned int total_count = 22;

int main(int argc, const char *argv[]) {
  //  NOTE: Check the number of arguments and print usage if needed
  if (argc < 2) {
    printf("Usage: assignment <number-of-threads>\n");
    exit(EXIT_FAILURE);
  }

  //  NOTE: Read and parse the number of threads from the command line
  int thread_count;
  if (!parse_int(argv[1], &thread_count) || thread_count < 1) {
    printf("Invalid number of threads: %s\n", argv[1]);
    exit(EXIT_FAILURE);
  }
  pthread_t threads[thread_count];

  //  NOTE: Read the input range from the file
  const char *filepath = "in.txt";
  FILE *input = fopen(filepath, "r");
  if (input == NULL) {
    printf("Cannot open file: %s\n", filepath);
    exit(EXIT_FAILURE);
  }

  unsigned int start, end;
  if (fscanf(input, "%d %d", &start, &end) != 2) {
    printf("Invalid input file format\n");
    fclose(input);
    exit(EXIT_FAILURE);
  }
  fclose(input);
  start = 0;
  end = 100;

  int range_size = (end - start + 1) / thread_count;
  int remainder = (end - start + 1) % thread_count;
  int current_start = start;

  for (int i = 0; i < thread_count; ++i) {
    int _start = current_start;
    int _end = current_start + range_size - 1;
    if (i < remainder) {
      _end++; 
    }

    int range[2] = {_start, _end};

    current_start = _end + 1;

    int failed = pthread_create(&threads[i], NULL, handle, (void *)range);
    if (failed) {
      printf("Error creating thread %d\n", i);
      exit(EXIT_FAILURE);
    }

    printf("ThreadID=%d, startNum=%d, endNum=%d\n", i, range[0], range[1]);
  }

  //  NOTE: Wait for all threads to finish
  for (int i = 0; i < thread_count; ++i) {
    pthread_join(threads[i], NULL);
  }

  printf("totalNums=%d, numOfPrime=%d, numOfPalindrome=%d, "
         "numOfPalindromicPrime=%d\n",
         total_count, prime_count, palindrome_count, palindromic_prime_count);

  return 0;
}

void *handle(void *arg) {
  int *range = (int *)arg;
  int start = range[0];
  int end = range[1];

  for (int i = start; i <= end; ++i) {
    total_count++;

    bool _is_prime = false;
    bool _is_palindrome = false;

    if (is_prime(i)) {
      _is_prime = true;
      // primes.push_back(i);
      prime_count++;
    }

    if (is_palindrome(i)) {
      _is_palindrome = true;
      // palindromes.push_back(i);
      palindrome_count++;
    }

    if (_is_prime && _is_palindrome) {
      // palindromic_primes.push_back(i);
      palindromic_prime_count++;
    }
  }

  return NULL;
}

#endif
#endif

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
