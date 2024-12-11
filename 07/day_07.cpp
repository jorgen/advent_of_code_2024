#include <cstdio>
#include <string>
#include <format>
#include <vector>
#include <optional>

template <typename T>
auto parse_int(const std::string_view &sv) -> std::optional<T>
{
  if (sv[0] == ' ' || sv.back() == ' ')
  {
    return std::nullopt;
  }

  T result = 0;
  auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), result);

  if (ec == std::errc())
    return result;

  return std::nullopt; // Return empty optional on error
}

std::vector<std::string_view> delimit(const std::string_view data, const char delimiter)
{
  std::vector<std::string_view> result;
  size_t next_delimiter = data.find(delimiter);
  size_t current_pos = 0;
  while (next_delimiter != std::string::npos)
  {
    result.emplace_back(data.substr(current_pos, next_delimiter - current_pos));
    current_pos = next_delimiter + 1;
    next_delimiter = data.find(delimiter, current_pos);
  }
  if (current_pos < data.size())
  {
    result.emplace_back(data.substr(current_pos));
  }
  return result;
}

bool plus_multiply_can_make_valid_equation(const std::pair<int64_t, std::vector<int>> &equation)
{
  int permutations = 1 << (equation.second.size() - 1);
  for (int i = 0; i < permutations; i++)
  {
    int64_t sum = equation.second[0];
    for (int j = 0; j < equation.second.size() - 1; j++)
    {
      if (i & (1 << j))
      {
        sum += equation.second[j + 1];
      }
      else
      {
        sum *= equation.second[j + 1];
      }
    }
    if (sum == equation.first)
    {
      return true;
    }
  }
  return false;
}


int64_t concat(const int64_t a, const int64_t b)
{
  int shift = int(log10(b)) + 1;
  auto multiplier = int64_t(pow(10, shift));
  return a * multiplier + b;
}

// In debug this is fast enough, but in we could optimize it by not calculating the entire permutation per itteration
bool plus_multiply_concat_can_make_valid_equation(const std::pair<int64_t, std::vector<int>> &equation)
{
  auto permutations = int(pow(3, equation.second.size() - 1));

  for (int i = 0; i < permutations; i++)
  {
    int64_t sum = equation.second[0];
    for (int j = 0; sum <= equation.first && j < equation.second.size() - 1; j++)
    {
      int op = (i / int(pow(3, j))) % 3;
      if (op == 0)
      {
        sum = concat(sum, equation.second[j + 1]);
      }
      else if (op == 1)
      {
        sum *= equation.second[j + 1];
      }
      else
      {
        sum += equation.second[j + 1];
      }
    }
    if (sum == equation.first)
    {
      return true;
    }
  }

  return false;
}


auto parse_file(const char *filename) -> std::pair<int64_t, int64_t>
{
  std::FILE *file = std::fopen(filename, "rb");
  if (!file)
    abort();

  std::fseek(file, 0, SEEK_END);
  auto size = std::ftell(file);
  std::rewind(file);

  std::string content(size, '\0');
  size_t read = 0;
  size_t local_read = 1;
  while (local_read > 0 && read < size)
  {
    local_read = std::fread(content.data() + read, 1, size - read, file);
    read += local_read;
  }
  if (read != size)
    abort();
  std::fclose(file);

  auto lines = delimit(content, '\n');
  std::vector<std::pair<int64_t, std::vector<int>>> equations;

  for (auto &line : lines)
  {
    auto sum_and_digits = delimit(line, ':');
    if (sum_and_digits.size() != 2)
    {
      abort();
    }
    auto sum = parse_int<int64_t>(sum_and_digits[0]);
    if (!sum.has_value())
    {
      abort();
    }
    auto &equation = equations.emplace_back(sum.value(), std::vector<int>());
    auto constants = delimit(sum_and_digits[1], ' ');
    for (auto &constant : constants)
    {
      if (constant.empty())
      {
        continue;
      }
      auto value = parse_int<int>(constant);
      if (!value.has_value())
      {
        abort();
      }
      equation.second.emplace_back(value.value());
    }
  }

  auto part_one = int64_t(0);

  for (auto &equation : equations)
  {
    if (plus_multiply_can_make_valid_equation(equation))
    {
      part_one += equation.first;
    }
  }

  auto part_two = int64_t(0);
  for (auto &equation : equations)
  {
    if (plus_multiply_concat_can_make_valid_equation(equation))
    {
      part_two += equation.first;
    }
  }

  return {part_one, part_two};
}

int main()
{
  auto [part_one, part_two] = parse_file(CURRENT_SOURCE_DIR "/input");
  std::printf("%s\n", std::format("part one: {}, part two: {}", part_one, part_two).c_str());
  return 0;
}
