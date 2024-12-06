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
  if (sv.size() < 1 || sv.size() > 3 || sv[0] == ' ' || sv.back() == ' ')
  {
    return std::nullopt;
  }
  int result = 0;
  auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), result);

  if (ec == std::errc())
    return result;

  return std::nullopt; // Return empty optional on error
}

void find_muls_in_section(const std::string_view &data, std::vector<std::string_view> &results)
{
  size_t start = 0;
  while(true)
  {
    auto mul_start = data.find("mul(", start);
    if (mul_start == std::string_view::npos) {
      return;
    }
    auto mul_end = data.find(')', mul_start);
    if (mul_end == std::string_view::npos) {
      return;
    }
    auto next_mul = data.find("mul(", mul_start + 4);
    if (next_mul < mul_end)
    {
      start = next_mul;
      continue;
    }
    results.push_back(data.substr(mul_start, mul_end - mul_start + 1));
    start = mul_end + 1;
  }
}
std::vector<std::string_view> find_all_muls(const std::string_view &data) {
  std::vector<std::string_view> results;
  size_t start = 0;
  std::vector<std::string_view> sections;
  while (true)
  {
    auto dont_start = data.find("don't()", start);
    if (dont_start == std::string_view::npos)
    {
      sections.emplace_back(data.substr(start));
      break;
    }
    sections.emplace_back(data.substr(start, dont_start - start));
    auto dont_end = data.find("do()", dont_start + 7);
    if (dont_end == std::string_view::npos)
    {
      break;
    }
    start = dont_end + 4;
  }
  for (const auto &section : sections)
  {
    find_muls_in_section(section, results);
  }
  return results;
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

  auto acc = std::accumulate(results.begin(), results.end(), 0);
  int sum = 0;
  for (int i = 0; i < results.size(); i++)
  {
    sum += results[i];
  }
  return sum;
}

int main()
{
  auto result = parse_file(CURRENT_SOURCE_DIR "/input");
  std::printf("muls: %d\n", result);
  return 0;
}
