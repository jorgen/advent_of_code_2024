#include <cstdio>
#include <string>
#include <vector>
#include <charconv>
#include <tuple>
#include <optional>
#include <memory>
#include <format>

struct result_t
{
  std::vector<int> row;
};

auto parse_int(std::string_view sv) -> std::optional<int>
{
  int result = 0;
  auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), result);

  if (ec == std::errc())
    return result;

  return std::nullopt; // Return empty optional on error
}

auto trim(std::string_view sv) -> std::string_view
{
  auto begin = sv.find_first_not_of(' ');
  if (begin == std::string_view::npos)
    return {}; // All spaces or empty string

  auto end = sv.find_last_not_of(' ');
  return sv.substr(begin, end - begin + 1);
}

bool is_line_safe(const std::vector<int> &line)
{
  if (line.size() < 2)
  {
    return false;
  }
  int prev_value = line[0];
  int inc = 0;
  int dec = 0;
  for (int i = 1; i < int(line.size()); i++)
  {
    inc += prev_value < line[i];
    dec += prev_value > line[i];
    auto diff = std::abs(prev_value - line[i]);
    prev_value = line[i];
    if (diff > 3 || diff < 1)
    {
      return false;
    }
  }

  return inc == 0 || dec == 0;
}

bool is_line_safe_with_damper(const std::vector<int> &line)
{
  if (is_line_safe(line))
  {
    return true;
  }
  for (int i = 0; i < int(line.size()); i++)
  {
    auto new_line = line;
    new_line.erase(new_line.begin() + i);
    if (is_line_safe(new_line))
    {
      return true;
    }
  }
  return false;
}

auto parse_file(const char* filename) -> std::vector<std::vector<int>>
{
  std::vector<std::vector<int>> results;

  std::FILE* file = std::fopen(filename, "r");
  if (!file) return results;

  std::fseek(file, 0, SEEK_END);
  auto size = std::ftell(file);
  std::rewind(file);

  std::string content(size, '\0');
  std::fread(content.data(), 1, size, file);
  std::fclose(file);

  std::size_t start = 0;
  while (start < content.size())
  {
    auto end = content.find('\n', start);
    if (end == std::string::npos) end = content.size();

    auto line = std::string_view(content).substr(start, end - start);
    auto &result_line = results.emplace_back();

    auto next_digit_start = line.begin();
    while (next_digit_start != line.end())
    {
      auto next_digit_end = std::find(next_digit_start, line.end(), ' ');
      auto digit = parse_int(trim(std::string_view(next_digit_start, next_digit_end)));
      if (!digit.has_value())
      {
        abort();
      }
      result_line.push_back(digit.value());
      next_digit_start = next_digit_end;
      if (next_digit_start != line.end())
      {
        next_digit_start++;
      }
    }
    start = end + 1;
  }

  return results;
}

int main()
{
  auto data = parse_file(CURRENT_SOURCE_DIR "/input");
  int safe = 0;
  for (const auto &line : data)
  {
    if (is_line_safe_with_damper(line))
    {
      safe++;
    }
  }
  std::printf("Safe lines: %d\n", safe);
  return 0;
}
