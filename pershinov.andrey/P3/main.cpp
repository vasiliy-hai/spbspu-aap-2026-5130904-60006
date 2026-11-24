#include <iostream>
#include <fstream>
#include <cstddef>

namespace pershinov {
  const int input_error = 1;
  const int exit_matrix_error = 2;
  size_t findColumn(const int * matrix, size_t rows, size_t cols)
  {
    size_t current_length = 0;
    size_t best_length = 0;
    size_t best_column = 0;
    for (size_t j = 0; j < cols; ++j) {
      for (size_t i = 0; i < rows; ++i) {
        if (i > 0 && matrix[(i - 1) * cols + j] == matrix[i * cols + j]) {
          ++current_length;
        } else {
          current_length = 1;
        }
        if (current_length > best_length) {
          best_length = current_length;
          best_column = j + 1;
        }
      }
    }
    return best_column;
  }
  void spiralDecrease(int * matrix, size_t rows, size_t cols)
  {
    size_t top = 0;
    size_t bottom = rows;
    size_t left = 0;
    size_t right = cols;
    int step = 1;
    while (top < bottom && left < right) {
      for (size_t j = left; j < right; ++j) {
        matrix[top * cols + j] -= step;
        ++step;
      }
      ++top;
      for (size_t i = top; i < bottom; ++i) {
        matrix[i * cols + right - 1] -= step;
        ++step;
      }
      --right;

      if (top < bottom) {
        for (size_t j = right; j > left; --j) {
          matrix[(bottom - 1) * cols + (j - 1)] -= step;
          ++step;
        }
        --bottom;
      }
      if (left < right) {
        for (size_t i = bottom; i > top; --i) {
          matrix[(i - 1) * cols + left] -= step;
          ++step;
        }
        ++left;
      }
    }
  }
  int processMatrix(int * matrix, size_t rows, size_t cols, std::istream & in, std::ostream & out)
  {
    for (size_t i = 0; i < rows * cols; ++i) {
      in >> matrix[i];
    }
    if (!in) {
      std::cerr << "Error: invalid matrix\n";
      return pershinov::exit_matrix_error;
    }
    char extra = 0;
    if (in >> extra) {
      std::cerr << "Error: extra data in file\n";
      return pershinov::exit_matrix_error;
    }
    out << findColumn(matrix, rows, cols) << "\n";
    spiralDecrease(matrix, rows, cols);
    out << rows << " " << cols;
    for (size_t i = 0; i < rows * cols; ++i) {
      out << " " << matrix[i];
    }
    out << "\n";
    return 0;
  }
}
int main(int argc, char ** argv)
{
  const int argc_element = 4;
  const char mode_stack = '1';
  const char mode_heap = '2';
  if (argc != argc_element) {
    std::cerr << "Error: wrong arguments\n";
    return pershinov::input_error;
  }
  const char * const mode = argv[1];
  const char * const input_file = argv[2];
  const char * const output_file = argv[3];

  if (!((mode[0] == mode_stack || mode[0] == mode_heap) && mode[1] == '\0')) {
    std::cerr << "Error: wrong mode\n";
    return pershinov::input_error;
  }
  std::ifstream input(input_file);
  if (!input) {
    std::cerr << "Error: could not open input file\n";
    return pershinov::input_error;
  }
  size_t cols = 0;
  size_t rows = 0;
  std::ofstream output(output_file);
  if (!output) {
    std::cerr << "Error: could not open output file\n";
    return pershinov::input_error;
  }
  input >> rows >> cols;
  if (!input) {
    std::cerr << "Error: invalid matrix\n";
    return pershinov::exit_matrix_error;
  }
  if (mode[0] == mode_stack) {
    const size_t big_size = 10000;
    if (rows * cols > big_size) {
      std::cerr << "Error: matrix too large\n";
      return pershinov::exit_matrix_error;
    }
    int stack_matrix[big_size];
    return pershinov::processMatrix(stack_matrix, rows, cols, input, output);
  }
  if (mode[0] == mode_heap) {
    int * const heap_matrix = new int[rows * cols];
    const int status = pershinov::processMatrix(heap_matrix, rows, cols, input, output);
    delete[] heap_matrix;
    return status;
  }
  return 0;
}

