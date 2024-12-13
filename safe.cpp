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

#define lock(mutex)                                                            \
  do {                                                                         \
    int failed = pthread_mutex_lock(mutex);                                    \
    if (failed) {                                                              \
      printf("Error locking mutex %s (code %d)\n", #mutex, failed);            \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

#define unlock(mutex)                                                          \
  do {                                                                         \
    int failed = pthread_mutex_unlock(mutex);                                  \
    if (failed) {                                                              \
      printf("Error unlocking mutex %s (code %d)\n", #mutex, failed);          \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

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
unsigned int total_count;

pthread_mutex_t primes_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t palindromes_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t palindromic_primes_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t prime_count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t palindrome_count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t palindromic_prime_count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t total_count_mutex = PTHREAD_MUTEX_INITIALIZER;

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

  int range_size = (end - start + 1) / thread_count;
  int remainder = (end - start + 1) % thread_count;
  int current_start = start;

  for (int i = 0; i < thread_count; ++i) {
    int _start = current_start;
    int _end = current_start + range_size; // - 1;
    if (i == 0) _end--;
    if (i < remainder) _end++;


    //  NOTE: Not allocating memory on the stack, because the memory address might be reused in 
    //  the next iteration before the thread finishes processing so we need to allocate memory on the heap.
    int *range = (int *)malloc(2 * sizeof(int)); 
    if (range == NULL) {
      printf("Memory allocation failed\n");
      exit(EXIT_FAILURE);
    }

    range[0] = _start;
    range[1] = _end;

    int failed = pthread_create(&threads[i], NULL, handle, (void *)range);
    if (failed) {
      printf("Error creating thread %d\n", i);
      exit(EXIT_FAILURE);
    }

    current_start = _end; //+ 1;

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

  free(range);

  unsigned int _prime_count = 0;
  unsigned int _palindrome_count = 0;
  unsigned int _palindromic_count = 0;
  unsigned int _total_count = 0;

  for (int i = start; i < end; ++i) {
    _total_count++;

    bool _is_prime = false;
    bool _is_palindrome = false;

    if (is_prime(i)) {
      _is_prime = true;
      _prime_count++;
    }

    if (is_palindrome(i)) {
      _is_palindrome = true;
      _palindrome_count++;
    }

    if (_is_prime && _is_palindrome) {
      _palindromic_count++;
    }
  }

  lock(&total_count_mutex);
  total_count += _total_count;
  unlock(&total_count_mutex);

  lock(&prime_count_mutex);
  prime_count += _prime_count;
  unlock(&prime_count_mutex);

  lock(&palindrome_count_mutex);
  palindrome_count += _palindrome_count;
  unlock(&palindrome_count_mutex);

  lock(&palindromic_primes_mutex);
  palindromic_prime_count += _palindromic_count;
  unlock(&palindromic_primes_mutex);

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
