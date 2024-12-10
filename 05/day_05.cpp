#include <cstdio>
#include <string>
#include <format>
#include <unordered_map>
#include <__charconv/from_chars_integral.h>

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

std::unordered_map<int, std::vector<int>> parse_rules(const std::string_view &rules_data)
{
  std::unordered_map<int, std::vector<int>> rules;
  size_t next_newline = rules_data.find('\n');
  while (next_newline != std::string::npos)
  {
    auto rule = rules_data.substr(0, next_newline);
    size_t delimiter = rule.find('|');
    if (delimiter == std::string::npos)
    {
      abort();
    }
    auto rule_number = parse_int(rule.substr(0, delimiter));
    auto rule_value = parse_int(std::string(rule.substr(delimiter + 1)));
    if (!rule_number.has_value() || !rule_value.has_value())
    {
      abort();
    }
    rules[rule_number.value()].emplace_back(rule_value.value());
    next_newline = rules_data.find('\n');
  }
  return rules;
}

std::vector<int> parse_updates(const std::string_view &updates_data)
{
  std::vector<std::vector<int>> updates;
  size_t next_newline = updates_data.find('\n');
  size_t current_pos = 0;
  while (next_newline != std::string::npos)
  {
    auto line = updates_data.substr(current_pos, next_newline);
    
    next_newline = updates_data.find('\n');
  }
  return updates;
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

  size_t newline_newline = content.find("\n\n");
  if (newline_newline== std::string::npos)
  {
    abort();
  }
  auto rules_data = std::string_view(content).substr(0, newline_newline);
  auto updats_data = std::string_view(content).substr(newline_newline + 2);

  rules_data.

  return 44;
}

int main()
{
  auto result = parse_file(CURRENT_SOURCE_DIR "/input");
  std::printf("xmas_found: %d\n", result);
  return 0;
}
