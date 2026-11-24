#include <iostream>
#include <fstream>
#include <limits>
#include <new>
#include <stdexcept>

namespace dortman
{
  const int err_args = 1;
  const int err_data = 2;
  const int expected_argc = 4;
  const int mode_fixed = 1;
  const int mode_dynamic = 2;
  const size_t max_fixed_size = 10000;

  int parseMode(const char* str)
  {
    if (str == nullptr)
    {
      return 0;
    }
    if (str[0] == '1' && str[1] == '\0')
    {
      return mode_fixed;
    }
    if (str[0] == '2' && str[1] == '\0')
    {
      return mode_dynamic;
    }
    return 0;
  }

  int safeAdd(int a, int b)
  {
    if (b > 0 && a > std::numeric_limits< int >::max() - b)
    {
      throw std::overflow_error("Переполнение при сложении целых чисел");
    }
    if (b < 0 && a < std::numeric_limits< int >::min() - b)
    {
      throw std::overflow_error("Переполнение при сложении целых чисел");
    }
    return a + b;
  }

  void addStep(int& val, int& step)
  {
    val = safeAdd(val, step);
    if (step == std::numeric_limits< int >::max())
    {
      throw std::overflow_error("Переполнение шага спирали");
    }
    ++step;
  }

  std::istream& readMatrix(std::istream& in, int* matrix, size_t rows, size_t cols)
  {
    for (size_t i = 0; i < rows * cols; ++i)
    {
      if (!(in >> matrix[i]))
      {
        return in;
      }
    }
    return in;
  }

  int minSumMdg(const int* matrix, size_t rows, size_t cols)
  {
    const size_t dim = (rows < cols) ? rows : cols;
    if (dim == 0)
    {
      return 0;
    }
    if (dim == 1)
    {
      return matrix[0];
    }

    if (dim - 1 > std::numeric_limits< size_t >::max() / 2)
    {
      throw std::overflow_error("Переполнение при расчете индексов диагоналей");
    }

    bool has_min = false;
    int min_sum = 0;
    const size_t max_s = 2 * (dim - 1);

    for (size_t s = 0; s <= max_s; ++s)
    {
      if (s == dim - 1)
      {
        continue;
      }
      int current_sum = 0;
      for (size_t i = 0; i < dim; ++i)
      {
        if (s >= i && (s - i) < dim)
        {
          const size_t j = s - i;
          const int val = matrix[i * cols + j];
          current_sum = safeAdd(current_sum, val);
        }
      }
      if (!has_min || current_sum < min_sum)
      {
        min_sum = current_sum;
        has_min = true;
      }
    }
    return min_sum;
  }

  void lftTopCnt(int* matrix, size_t rows, size_t cols)
  {
    if (rows == 0 || cols == 0)
    {
      return;
    }

    size_t top = 0;
    size_t bottom = rows - 1;
    size_t left = 0;
    size_t right = cols - 1;
    int step = 1;

    while (top <= bottom && left <= right)
    {
      for (size_t i = top; i <= bottom; ++i)
      {
        addStep(matrix[i * cols + left], step);
      }
      left++;
      if (left > right)
      {
        break;
      }

      for (size_t j = left; j <= right; ++j)
      {
        addStep(matrix[bottom * cols + j], step);
      }
      if (bottom == 0)
      {
        break;
      }
      bottom--;
      if (top > bottom)
      {
        break;
      }

      for (size_t i = bottom + 1; i-- > top;)
      {
        addStep(matrix[i * cols + right], step);
      }
      if (right == 0)
      {
        break;
      }
      right--;
      if (left > right)
      {
        break;
      }

      for (size_t j = right + 1; j-- > left;)
      {
        addStep(matrix[top * cols + j], step);
      }
      top++;
    }
  }

  void printMatrix(std::ostream& out, const int* matrix, size_t rows, size_t cols)
  {
    out << rows << " " << cols;
    for (size_t i = 0; i < rows * cols; ++i)
    {
      out << " " << matrix[i];
    }
  }

  int processMatrix(std::istream& input, const char* out_filename, int* matrix, size_t rows, size_t cols)
  {
    const size_t total = rows * cols;
    if (total > 0 && !readMatrix(input, matrix, rows, cols))
    {
      std::cerr << "Не удалось прочитать элементы матрицы\n";
      return err_data;
    }

    char extra = '\0';
    if (input >> extra)
    {
      std::cerr << "Обнаружены лишние данные после элементов матрицы\n";
      return err_data;
    }

    int res15 = 0;
    try
    {
      res15 = minSumMdg(matrix, rows, cols);
      lftTopCnt(matrix, rows, cols);
    }
    catch (const std::overflow_error& e)
    {
      std::cerr << e.what() << "\n";
      return err_data;
    }

    std::ofstream output(out_filename);
    if (!output.is_open())
    {
      std::cerr << "Не удалось открыть выходной файл для записи\n";
      return err_data;
    }

    output << res15 << "\n";
    printMatrix(output, matrix, rows, cols);
    output << "\n";
    return 0;
  }
}

int main(int argc, char** argv)
{
  if (argc != dortman::expected_argc)
  {
    std::cerr << "Неверное количество аргументов командной строки\n";
    return dortman::err_args;
  }

  const int mode = dortman::parseMode(argv[1]);
  if (mode != dortman::mode_fixed && mode != dortman::mode_dynamic)
  {
    std::cerr << "Неверный первый параметр командной строки\n";
    return dortman::err_args;
  }

  std::ifstream input(argv[2]);
  if (!input.is_open())
  {
    std::cerr << "Не удалось открыть входной файл\n";
    return dortman::err_data;
  }

  long long r_in = 0;
  long long c_in = 0;
  if (!(input >> r_in >> c_in) || r_in < 0 || c_in < 0)
  {
    std::cerr << "Неверные размеры матрицы во входном файле\n";
    return dortman::err_data;
  }

  const size_t rows = static_cast< size_t >(r_in);
  const size_t cols = static_cast< size_t >(c_in);

  if (rows > 0 && cols > std::numeric_limits< size_t >::max() / rows)
  {
    std::cerr << "Переполнение размеров матрицы\n";
    return dortman::err_data;
  }

  const size_t total = rows * cols;
  if (mode == dortman::mode_fixed)
  {
    if (total > dortman::max_fixed_size)
    {
      std::cerr << "Размер матрицы превышает лимит для статического массива\n";
      return dortman::err_data;
    }
    int fixed_matrix[dortman::max_fixed_size];
    return dortman::processMatrix(input, argv[3], fixed_matrix, rows, cols);
  }

  int* dyn_matrix = nullptr;
  if (total > 0)
  {
    try
    {
      dyn_matrix = new int[total];
    }
    catch (const std::bad_alloc&)
    {
      std::cerr << "Ошибка выделения динамической памяти\n";
      return dortman::err_data;
    }
  }

  const int status = dortman::processMatrix(input, argv[3], dyn_matrix, rows, cols);
  delete[] dyn_matrix;
  return status;
}
