#include <iostream>
#include <limits>

namespace bychkova {
  constexpr int inputErrorCode = 1;
  constexpr int errorSum = 2;
}

int main()
{
  int current_value = 0;
  int previous_value = 0;
  int last_non_zero = 0;

  std::size_t current_run_length = 0;
  std::size_t max_run_length = 0;
  std::size_t sign_changes = 0;
  bool run_length_error = false;
  bool sign_changes_error = false;

  while (std::cin >> current_value && current_value != 0) {
    if (current_value >= previous_value) {
      if (current_run_length == std::numeric_limits< std::size_t >::max()) {
        run_length_error = true;
      } else {
        ++current_run_length;
      }
    } else {
      current_run_length = 1;
    }

    if (current_run_length > max_run_length) {
      max_run_length = current_run_length;
    }

    if (last_non_zero != 0 && ((last_non_zero < 0 && current_value > 0) || (last_non_zero > 0 && current_value < 0))) {
      if (sign_changes == std::numeric_limits< std::size_t >::max()) {
        sign_changes_error = true;
      } else {
        ++sign_changes;
      }
    }

    previous_value = current_value;
    last_non_zero = current_value;
  }

  if (!std::cin) {
    std::cerr << "Error: Invalid input data or missing zero terminator." << std::endl;
    return bychkova::inputErrorCode;
  }

  if (run_length_error) {
    std::cerr << "Error: Increasing fragment is too long." << std::endl;
  } else {
    std::cout << max_run_length << std::endl;
  }

  if (sign_changes_error) {
    std::cerr << "Error: Too many sign changes." << std::endl;
  } else {
    std::cout << sign_changes << std::endl;
  }

  if (run_length_error || sign_changes_error) {
    return bychkova::errorSum;
  }

  return 0;
}
