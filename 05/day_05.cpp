#include <cstdio>
#include <string>
#include <format>
#include <unordered_map>
#include <optional>
#include <charconv>

auto parse_int(const std::string_view &sv) -> std::optional<int>
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

std::unordered_map<int, std::vector<int>> parse_rules(const std::string_view &rules_data)
{
  std::unordered_map<int, std::vector<int>> rules;
  for (const auto lines = delimit(rules_data, '\n'); auto &line : lines)
  {
    auto rule = delimit(line, '|');
    if (rule.size() != 2)
    {
      abort();
    }
    auto rule_number = parse_int(rule[0]);
    auto rule_value = parse_int(rule[1]);
    if (!rule_number.has_value() || !rule_value.has_value())
    {
      abort();
    }
    rules[rule_number.value()].emplace_back(rule_value.value());
  }
  return rules;
}

std::vector<std::vector<int>> parse_updates(const std::string_view &updates_data)
{
  std::vector<std::vector<int>> updates;
  for (const auto lines = delimit(updates_data, '\n'); const auto line: lines) {
    if (auto number_strings = delimit(line, ','); !number_strings.empty())
    {
      auto &update = updates.emplace_back();
      for (const auto number: number_strings) {
        auto value = parse_int(number);
        if (!value.has_value()) {
          abort();
        }
        update.emplace_back(value.value());
      }
    }
  }
  return updates;
}

bool validate_update(const std::unordered_map<int, std::vector<int> > &rules, const std::vector<int> &updates) {
  for (auto it = updates.begin(); it != updates.end(); ++it) {
    auto rule = rules.find(*it);
    if (rule == rules.end()) {
      continue;
    }
    for (auto after: rule->second) {
      if (auto found_before = std::find(updates.begin(), it, after); found_before != it) {
        return false;
      }
    }
  }
  return true;
}


template <typename t> void move(std::vector<t>& v, size_t oldIndex, size_t newIndex)
{
  if (oldIndex > newIndex)
    std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex, v.rend() - newIndex);
  else
    std::rotate(v.begin() + oldIndex, v.begin() + oldIndex + 1, v.begin() + newIndex + 1);
}

bool validate_and_fix_update(const std::unordered_map<int, std::vector<int> > &rules, std::vector<int> &updates) {
  bool fixed = false;
  for (auto it = updates.begin(); it != updates.end(); ++it) {
    auto rule = rules.find(*it);
    if (rule == rules.end()) {
      continue;
    }
    for (auto after: rule->second) {
      if (auto found_before = std::find(updates.begin(), it, after); found_before != it) {
        it = --std::rotate(found_before, it, it + 1);
        fixed = true;
      }
    }
  }
  return fixed;
}

auto parse_file(const char* filename) -> std::pair<int, int>
{
  std::FILE *file = std::fopen(filename, "rb");
  if (!file) abort();

  std::fseek(file, 0, SEEK_END);
  auto size = std::ftell(file);
  std::rewind(file);

  std::string content(size, '\0');
  size_t read = 0;
  size_t local_read = 1;
  while (local_read > 0 && read < size) {
    local_read = std::fread(content.data()+ read, 1, size - read, file);
    read += local_read;
  }
  if (read != size) abort();
  std::fclose(file);

  size_t newline_newline = content.find("\n\n");
  if (newline_newline== std::string::npos)
  {
    abort();
  }
  const auto rules_data = std::string_view(content).substr(0, newline_newline);
  const auto updates_data = std::string_view(content).substr(newline_newline + 2);
  const auto updates = parse_updates(updates_data);
  const auto rules = parse_rules(rules_data);

  auto middle_number_sum = 0;

  for (auto &update: updates)
  {
    if (validate_update(rules, update))
    {
      if (!(update.size() & 1))
      {
        abort();
      }
      middle_number_sum += update[update.size() / 2];
    }
  }

  auto middle_number_fixed_sum = 0;
  for (auto &update : updates)
  {
    auto mod_update = update;
    if (validate_and_fix_update(rules, mod_update)) {
      if (!(update.size() & 1))
      {
        abort();
      }
      middle_number_fixed_sum += mod_update[mod_update.size() / 2];
    }
  }

  return {middle_number_sum, middle_number_fixed_sum};
}

int main()
{
  auto [first, second] = parse_file(CURRENT_SOURCE_DIR "/input");
  std::printf("%s\n", std::format("middle_number_sum: {}, middle_number_fixed_sum: {}\n", first, second).c_str());
  return 0;
}
