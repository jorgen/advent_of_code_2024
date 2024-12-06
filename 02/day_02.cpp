#include <cstdio>
#include <string>
#include <vector>
#include <charconv>
#include <tuple>
#include <optional>
#include <memory>
#include <format>
#include <numeric>

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

std::vector<std::string_view> find_all_muls(std::string_view data) {
  std::vector<std::string_view> results;
  while (true) {
    auto mul_start = data.find("mul(");
    if (mul_start == std::string_view::npos) {
      return {};
    }
    auto mul_end = data.find(')', mul_start);
    if (mul_end == std::string_view::npos) {
      return {};
    }
    results.push_back(data.substr(mul_start, mul_end - mul_start + 1));
  }
}

int evaluate_mul(const std::string_view &mul)
{
  auto content = mul.substr(4, mul.size() - 5);
  auto comma = content.find(',');
  if (comma == std::string_view::npos)
  {
    return 0;
  }
  auto first = parse_int(content.substr(0, comma));
  auto second = parse_int(content.substr(comma + 1));
  if (!first.has_value() || !second.has_value())
  {
    return 0;
  }
  return first.value() * second.value();
}

auto parse_file(const char* filename) -> int
{

  std::FILE* file = std::fopen(filename, "r");
  if (!file) abort();

  std::fseek(file, 0, SEEK_END);
  auto size = std::ftell(file);
  std::rewind(file);

  std::string content(size, '\0');
  std::fread(content.data(), 1, size, file);
  std::fclose(file);

  auto muls = find_all_muls(content);
  std::vector<int> results;
  for (const auto &mul : muls)
  {
    results.push_back({evaluate_mul(mul)});
  }

  return std::accumulate(results.begin(), results.end(), 0);

}

int main()
{
  auto result = parse_file(CURRENT_SOURCE_DIR "/input");
  std::printf("muls: %d\n", result);
  return 0;
}
