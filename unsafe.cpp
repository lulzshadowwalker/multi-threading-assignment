#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#define OUT

class DynamicArray {
private:
  int *data;
  size_t capacity;
  size_t size;

  void resize(size_t newCapacity) {
    int *newData = new int[newCapacity];
    for (size_t i = 0; i < size; ++i) {
      newData[i] = data[i];
    }
    delete[] data;
    data = newData;
    capacity = newCapacity;
  }

public:
  DynamicArray(size_t initialCapacity = 4)
      : data(new int[initialCapacity]), capacity(initialCapacity), size(0) {}

  ~DynamicArray() { delete[] data; }

  size_t getSize() const { return size; }

  size_t getCapacity() const { return capacity; }

  void append(int value) {
    if (size == capacity) {
      resize(capacity * 2); 
    }
    data[size++] = value;
  }

  int get(size_t index) const {
    if (index >= size) {
      throw std::out_of_range("Index out of range");
    }
    return data[index];
  }

  int operator[](size_t index) const { return get(index); }
};

DynamicArray primes;
DynamicArray palindromes;
DynamicArray palindromic_primes;

bool parse_int(const char *str, OUT int *res);
bool is_prime(int n);
bool is_palindrome(int n);
void log(std::string message);

// handles the range of numbers assigned to the thread and counts the number of
// primes, palindromes, and palindromic primes
void *handle(void *arg);

#ifdef UNSAFE

unsigned int prime_count;
unsigned int palindrome_count;
unsigned int palindromic_prime_count;
unsigned int total_count;

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
    if (i == 0)
      _end--;
    if (i < remainder)
      _end++;

    //  NOTE: Not allocating memory on the stack, because the memory address
    //  might be reused in the next iteration before the thread finishes
    //  processing so we need to allocate memory on the heap.
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

    current_start = _end; // + 1;

    printf("ThreadID=%d, startNum=%d, endNum=%d\n", i, range[0], range[1]);
  }

  //  NOTE: Wait for all threads to finish
  for (int i = 0; i < thread_count; ++i) {
    pthread_join(threads[i], NULL);
  }

  printf("totalNums=%d, numOfPrime=%d, numOfPalindrome=%d, "
         "numOfPalindromicPrime=%d\n",
         total_count, prime_count, palindrome_count, palindromic_prime_count);

  //  NOTE: Remove output file if it already exists
  if (std::ifstream("./unsafe-out.txt"))
    remove("./unsafe-out.txt");

  std::ostringstream output;

  output << "The prime numbers are:\n";
  for (int prime = 0; prime < primes.getSize(); ++prime) {
    output << primes.get(prime) << '\n';
  }

  output << "The palindrome numbers are:\n";
  for (int palindrome = 0; palindrome < palindromes.getSize(); ++palindrome) {
    output << palindromes.get(palindrome) << '\n';
  }

  output << "The palindromicPrime numbers are:\n";
  for (int palindromic_prime = 0; palindromic_prime < palindromic_primes.getSize(); ++palindromic_prime) {
    output << palindromic_primes.get(palindromic_prime) << '\n';
  }

  log(output.str());

  return 0;
}

void *handle(void *arg) {
  int *range = (int *)arg;
  int start = range[0];
  int end = range[1];

  free(range);

  for (int i = start; i <= end; ++i) {
    total_count++;

    bool _is_prime = false;
    bool _is_palindrome = false;

    if (is_prime(i)) {
      _is_prime = true;
      prime_count++;
      primes.append(i);
    }

    if (is_palindrome(i)) {
      _is_palindrome = true;
      palindrome_count++;
      palindromes.append(i);
    }

    if (_is_prime && _is_palindrome) {
      palindromic_prime_count++;
      palindromic_primes.append(i);
    }
  }

  return NULL;
}

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

void log(std::string message) {
  if (message.back() != '\n')
    message += "\n";

  std::ofstream out;

  out.open("./unsafe-out.txt", std::ios::app);
  out << message;
  out.close();
}

#endif
