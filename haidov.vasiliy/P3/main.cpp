#include <iostream>
#include <fstream>
#include <limits>

const int correct_result = 0;
const int calculation_error = 2;
const int invalid_input = 1;
const size_t MAX_LEN = 10000;

namespace haidov {
  int sedlo(int* data, size_t n, size_t m)
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

  int local_min(int* data, size_t n, size_t m)
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
    if (argc != 4) {
      std::cerr << "Invalid arguments" << std::endl;
      return invalid_input;
    }
    int type = 0;
    if (argv[1] == nullptr) {
      std::cerr << "Invalid type" << std::endl;
      return invalid_input;
    } else if (argv[1][0] == '1' && argv[1][1] == '\0') {
      type = 1;
    } else if (argv[1][0] == '2' && argv[1][1] == '\0') {
      type = 2;
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
      throw calculation_error;
    }
    if (n_long < 0 || m_long < 0) {
      std::cerr << "Invalid matrix size, negative" << std::endl;
      throw calculation_error;
    }
    const size_t n = static_cast< size_t >(n_long);
    const size_t m = static_cast< size_t >(m_long);
    int* data = nullptr;
    int fixed_sized[MAX_LEN]{};
    if (type == 1) {
      if (n > MAX_LEN / m) {
        std::cerr << "Invalid matrix size, too big" << std::endl;
        return calculation_error;
      }
      data = fixed_sized;
    } else if (type == 2) {
      if (n > std::numeric_limits< size_t >::max() / m) {
        std::cerr << "Invalid matrix size, too big" << std::endl;
        return calculation_error;
      }
      try {
        data = new int[n * m];
      } catch (...) {
        std::cerr << "Memory allocation error";
        return calculation_error;
      }
    } else {
      std::cerr << "Invalid type argument" << std::endl;
      return invalid_input;
    }
    for (size_t i = 0; i < n * m; ++i) {
      if (!(input >> data[i])) {
        std::cerr << "Input is invalid" << std::endl;
        if (type == 2) {
          delete[] data;
        }
        return calculation_error;
      }
    }
    output << local_min(data, n, m) << "\n";
    output << sedlo(data, n, m) << "\n";
    if (type == 2) {
      delete[] data;
    }
    return correct_result;
  }
}

int main(int argc, char** argv)
{
  haidov::lab(argc, argv);
  return 0;
}
