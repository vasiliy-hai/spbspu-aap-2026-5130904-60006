#include <cstddef>
#include <exception>
#include <fstream>
#include <iostream>
#include <limits>

namespace aleshin {

  constexpr int success = 0;
  constexpr int argsError = 1;
  constexpr int inputError = 2;
  constexpr int expectedArgsCount = 4;
  constexpr int staticMode = 1;
  constexpr int dynamicMode = 2;
  constexpr std::size_t maxSize = 10000;

  bool parseMode(const char* arg, int& mode)
  {
    if (arg[0] == '\0') {
      return false;
    }
    if (arg[1] != '\0') {
      return false;
    }
    if (arg[0] == '1') {
      mode = staticMode;
      return true;
    }
    if (arg[0] == '2') {
      mode = dynamicMode;
      return true;
    }
    return false;
  }

  bool readSize(std::istream& input, std::size_t& rows, std::size_t& cols)
  {
    long long n = 0;
    long long m = 0;
    if (!(input >> n >> m)) {
      return false;
    }
    if (n < 0 || m < 0) {
      return false;
    }
    rows = static_cast< std::size_t >(n);
    cols = static_cast< std::size_t >(m);
    if (cols != 0 && rows > std::numeric_limits< std::size_t >::max() / cols) {
      return false;
    }
    return true;
  }

  bool readElements(std::istream& input, int* data, std::size_t count)
  {
    for (std::size_t i = 0; i < count; ++i) {
      if (!(input >> data[i])) {
        return false;
      }
    }
    return true;
  }

  bool hasExtraData(std::istream& input)
  {
    char extra = '\0';
    return (input >> extra).good();
  }

  long long diagonalSum(const int* data, std::size_t rows, std::size_t cols, std::size_t row, std::size_t col)
  {
    long long sum = 0;
    while (row < rows && col < cols) {
      sum += data[row * cols + col];
      ++row;
      ++col;
    }
    return sum;
  }

  long long maxSumParallelToMain(const int* data, std::size_t rows, std::size_t cols)
  {
    bool found = false;
    long long result = 0;
    for (std::size_t row = 1; row < rows; ++row) {
      const long long sum = diagonalSum(data, rows, cols, row, 0);
      if (!found || sum > result) {
        result = sum;
        found = true;
      }
    }
    for (std::size_t col = 1; col < cols; ++col) {
      const long long sum = diagonalSum(data, rows, cols, 0, col);
      if (!found || sum > result) {
        result = sum;
        found = true;
      }
    }
    return result;
  }

  bool isUpperTriangular(const int* data, std::size_t rows, std::size_t cols)
  {
    std::size_t size = rows;
    if (cols < size) {
      size = cols;
    }
    if (size == 0) {
      return false;
    }
    for (std::size_t row = 1; row < size; ++row) {
      for (std::size_t col = 0; col < row; ++col) {
        if (data[row * cols + col] != 0) {
          return false;
        }
      }
    }
    return true;
  }

  int process(std::istream& input, std::ostream& output, int* data, std::size_t rows, std::size_t cols)
  {
    if (!readElements(input, data, rows * cols)) {
      std::cerr << "Error: input file does not contain enough valid matrix elements\n";
      return inputError;
    }
    if (hasExtraData(input)) {
      std::cerr << "Error: input file contains extra dsta after the matrix\n";
      return inputError;
    }
    output << maxSumParallelToMain(data, rows, cols) << '\n';
    output << std::boolalpha << isUpperTriangular(data, rows, cols) << '\n';
    if (!output) {
      std::cerr << "Error: failed to write the result to the output file\n";
      return inputError;
    }
    return success;
  }

  int run(int mode, std::istream& input, std::ostream& output)
  {
    std::size_t rows = 0;
    std::size_t cols = 0;
    if (!readSize(input, rows, cols)) {
      std::cerr << "Error: input file does not contain a matrix\n";
      return inputError;
    }
    const std::size_t count = rows * cols;
    if (mode == staticMode) {
      if (count > maxSize) {
        std::cerr << "Error: matrix is too large for a fixed-size array\n";
        return inputError;
      }
      int buffer[maxSize];
      return process(input, output, buffer, rows, cols);
    }
    int* const data = new int[count];
    const int code = process(input, output, data, rows, cols);
    delete[] data;
    return code;
  }

  int runApplication(int argc, char** argv)
  {
    if (argc != expectedArgsCount) {
      std::cerr << "Error: usage: ./lab num input output\n";
      return argsError;
    }
    int mode = 0;
    if (!parseMode(argv[1], mode)) {
      std::cerr << "Error: the first parameter must be 1 or 2\n";
      return argsError;
    }
    std::ifstream input{argv[2]};
    if (!input) {
      std::cerr << "Error: cannot open the input file\n";
      return argsError;
    }
    std::ofstream output{argv[3]};
    if (!output) {
      std::cerr << "Error: cannot open the output file\n";
      return argsError;
    }
    return run(mode, input, output);
  }

}

int main(int argc, char** argv)
{
  try {
    return aleshin::runApplication(argc, argv);
  } catch (const std::exception& error) {
    std::cerr << "Error: " << error.what() << '\n';
    return aleshin::inputError;
  }
}
