#include <iostream>
#include <cstddef>
#include <fstream>
#include <new>
#include <stdexcept>

namespace podvalnyj {
  const std::size_t MAX_SIZE = 10000;
  const int ARGS_COUNT = 4;
  const int MODE_FIXED = 1;
  const int MODE_DYNAMIC = 2;
  const int ERROR_ARGS = 1;
  const int ERROR_INPUT = 2;

  void checkArgsCount(int argc)
  {
    if (argc < ARGS_COUNT) {
      throw std::logic_error("Not enough arguments");
    }
    if (argc > ARGS_COUNT) {
      throw std::logic_error("Too many arguments");
    }
  }

  bool isNumber(const char * argument)
  {
    if (argument[0] == '\0') {
      return false;
    }
    for (std::size_t i = 0; argument[i] != '\0'; ++i) {
      if (argument[i] < '0' || argument[i] > '9') {
        return false;
      }
    }
    return true;
  }

  int getMode(const char * argument)
  {
    if (!isNumber(argument)) {
      throw std::logic_error("First parameter is not a number");
    }
    const int mode = argument[0] - '0';
    if (argument[1] != '\0' || (mode != MODE_FIXED && mode != MODE_DYNAMIC)) {
      throw std::logic_error("First parameter is out of range");
    }
    return mode;
  }

  void readSize(std::ifstream & input, std::size_t & rows, std::size_t & cols)
  {
    rows = 0;
    cols = 0;
    int rows_count = 0;
    int cols_count = 0;
    input >> rows_count >> cols_count;
    if (input.fail() || rows_count < 0 || cols_count < 0) {
      throw std::logic_error("Invalid matrix size");
    }
    if ((rows_count == 0) != (cols_count == 0)) {
      throw std::logic_error("Invalid matrix size");
    }
    rows = static_cast< std::size_t >(rows_count);
    cols = static_cast< std::size_t >(cols_count);
  }

  void readMatrix(std::ifstream & input, int * matrix, std::size_t size)
  {
    for (std::size_t i = 0; i < size; ++i) {
      input >> matrix[i];
      if (input.fail()) {
        throw std::logic_error("Invalid matrix elements");
      }
    }
    char extra = 0;
    input >> extra;
    if (!input.eof()) {
      throw std::logic_error("Extra data in input file");
    }
  }

  int * createMatrix(std::size_t size)
  {
    return new int[size];
  }

  void deleteMatrix(const int * matrix)
  {
    delete[] matrix;
  }

  bool hasEqualNeighbours(const int * row, std::size_t cols)
  {
    for (std::size_t j = 1; j < cols; ++j) {
      if (row[j] == row[j - 1]) {
        return true;
      }
    }
    return false;
  }

  std::size_t longestSeries(const int * row, std::size_t cols)
  {
    if (cols == 0) {
      return 0;
    }
    std::size_t current = 1;
    std::size_t longest = 1;
    for (std::size_t j = 1; j < cols; ++j) {
      if (row[j] == row[j - 1]) {
        ++current;
      } else {
        current = 1;
      }
      if (current > longest) {
        longest = current;
      }
    }
    return longest;
  }

  std::size_t countRowsWithoutEqual(const int * matrix, std::size_t rows, std::size_t cols)
  {
    std::size_t count = 0;
    for (std::size_t i = 0; i < rows; ++i) {
      if (!hasEqualNeighbours(matrix + i * cols, cols)) {
        ++count;
      }
    }
    return count;
  }

  std::size_t rowWithLongestSeries(const int * matrix, std::size_t rows, std::size_t cols)
  {
    std::size_t number = 0;
    std::size_t longest = 0;
    for (std::size_t i = 0; i < rows; ++i) {
      const std::size_t length = longestSeries(matrix + i * cols, cols);
      if (length > longest) {
        longest = length;
        number = i + 1;
      }
    }
    return number;
  }

  void writeResults(const char * filename, const int * matrix, std::size_t rows, std::size_t cols)
  {
    std::ofstream output(filename);
    if (!output.is_open()) {
      throw std::logic_error("Cannot open output file");
    }
    output << rowWithLongestSeries(matrix, rows, cols) << "\n";
    output << countRowsWithoutEqual(matrix, rows, cols) << "\n";
    output.close();
    if (output.fail()) {
      throw std::logic_error("Cannot write output file");
    }
  }

  void processFixed(std::ifstream & input, const char * filename, std::size_t rows, std::size_t cols)
  {
    if (cols != 0 && rows > MAX_SIZE / cols) {
      throw std::logic_error("Matrix is too big");
    }
    int matrix[MAX_SIZE] = {};
    readMatrix(input, matrix, rows * cols);
    writeResults(filename, matrix, rows, cols);
  }

  void processDynamic(std::ifstream & input, const char * filename, std::size_t rows, std::size_t cols)
  {
    int * const matrix = createMatrix(rows * cols);
    try {
      readMatrix(input, matrix, rows * cols);
      writeResults(filename, matrix, rows, cols);
    } catch (...) {
      deleteMatrix(matrix);
      throw;
    }
    deleteMatrix(matrix);
  }

  void process(int mode, const char * input_name, const char * output_name)
  {
    std::ifstream input(input_name);
    if (!input.is_open()) {
      throw std::logic_error("Cannot open input file");
    }
    std::size_t rows = 0;
    std::size_t cols = 0;
    readSize(input, rows, cols);
    if (mode == MODE_FIXED) {
      processFixed(input, output_name, rows, cols);
    } else {
      processDynamic(input, output_name, rows, cols);
    }
  }
}

int main(int argc, char ** argv)
{
  int mode = 0;
  try {
    podvalnyj::checkArgsCount(argc);
    mode = podvalnyj::getMode(argv[1]);
  } catch (const std::logic_error & e) {
    std::cerr << e.what() << "\n";
    return podvalnyj::ERROR_ARGS;
  }

  try {
    podvalnyj::process(mode, argv[2], argv[3]);
  } catch (const std::logic_error & e) {
    std::cerr << e.what() << "\n";
    return podvalnyj::ERROR_INPUT;
  } catch (const std::bad_alloc & e) {
    std::cerr << "Not enough memory\n";
    return podvalnyj::ERROR_INPUT;
  }
  return 0;
}
