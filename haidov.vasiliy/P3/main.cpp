#include <iostream>
#include <fstream>
#include <limits>

namespace haidov {
  const int correct_result = 0;
  const int calculation_error = 2;
  const int invalid_input = 1;
  const size_t MAX_LEN = 10000;
  const int UNDEFINED = 0;
  const int TYPE_STATIC = 1;
  const int TYPE_DYNAMIC = 2;
  const int CORRECT_ARGUMENT_AMOUNT = 4;

  int sedlo(const int* data, size_t n, size_t m)
  {
    int result = 0;
    for (size_t i = 0; i < n; ++i) {
      int min = std::numeric_limits< int >::max();
      int min_id = 0;
      for (size_t j = 0; j < m; ++j) {
        if (data[i * m + j] < min) {
          min = data[i * m + j];
          min_id = j;
        }
      }
      bool check = true;
      for (size_t k = 0; k < n; ++k) {
        if (data[k * m + min_id] > min) {
          check = false;
        }
      }
      result += check ? 1 : 0;
    }
    return result;
  }

  int localMin(const int* data, size_t n, size_t m)
  {
    int result = 0;
    for (size_t i = 1; i < n - 1; ++i) {
      for (size_t j = 0; j < m - 1; ++j) {
        if (data[i * m + j] < data[(i - 1) * m + j] && data[i * m + j] < data[i * (m + 1) + j]
            && data[i * m + j] < data[i * m + j + 1] && data[i * m + j] < data[i * m + j - 1]) {
          result += 1;
        }
      }
    }
    return result;
  }

  int lab(int argc, char** argv)
  {
    if (argc != CORRECT_ARGUMENT_AMOUNT) {
      std::cerr << "Invalid arguments" << std::endl;
      return invalid_input;
    }
    int type = UNDEFINED;
    if (argv[1] == nullptr) {
      std::cerr << "Invalid type" << std::endl;
      return invalid_input;
    } else if (argv[1][0] == '1' && argv[1][1] == '\0') {
      type = TYPE_STATIC;
    } else if (argv[1][0] == '2' && argv[1][1] == '\0') {
      type = TYPE_DYNAMIC;
    } else {
      std::cerr << "Invalid type" << std::endl;
      return invalid_input;
    }
    std::ifstream input{argv[2]};
    if (!input) {
      std::cerr << "Invalid input file path" << std::endl;
      return invalid_input;
    }
    std::ofstream output{argv[3]};
    if (!output) {
      std::cerr << "Invalid output file path" << std::endl;
      return invalid_input;
    }
    long long n_long = 0;
    long long m_long = 0;
    if (!(input >> n_long >> m_long)) {
      std::cerr << "Invalid data in input file" << std::endl;
      return calculation_error;
    }
    if (n_long < 0 || m_long < 0) {
      std::cerr << "Invalid matrix size, negative" << std::endl;
      return calculation_error;
    }
    const size_t n = static_cast< size_t >(n_long);
    const size_t m = static_cast< size_t >(m_long);
    if (n == 0 || m == 0) {
      output << "0 0\n0\n";
      return correct_result;
    }
    if (type == TYPE_STATIC) {
      if (n > MAX_LEN / m) {
        std::cerr << "Invalid matrix size, too big" << std::endl;
        return calculation_error;
      }
      int fixed_sized[MAX_LEN]{};
      for (size_t i = 0; i < n * m; ++i) {
        if (!(input >> fixed_sized[i])) {
          std::cerr << "Input is invalid" << std::endl;
          return calculation_error;
        }
      }
      output << localMin(fixed_sized, n, m) << "\n";
      output << sedlo(fixed_sized, n, m) << "\n";
    } else if (type == TYPE_DYNAMIC) {
      if (n > std::numeric_limits< size_t >::max() / m) {
        std::cerr << "Invalid matrix size, too big" << std::endl;
        return calculation_error;
      }
      int* data = nullptr;
      try {
        data = new int[n * m];
      } catch (...) {
        std::cerr << "Memory allocation error";
        return calculation_error;
      }
      for (size_t i = 0; i < n * m; ++i) {
        if (!(input >> data[i])) {
          std::cerr << "Input is invalid" << std::endl;
          delete[] data;
          return calculation_error;
        }
      }
      output << localMin(data, n, m) << "\n";
      output << sedlo(data, n, m) << "\n";
      delete[] data;
    } else {
      std::cerr << "Invalid type argument" << std::endl;
      return invalid_input;
    }
    return correct_result;
  }
}

int main(int argc, char** argv)
{
  return haidov::lab(argc, argv);
}
