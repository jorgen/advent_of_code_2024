#include <cstdio>
#include <string>
#include <vector>
#include <charconv>
#include <tuple>
#include <optional>
#include <memory>
#include <algorithm>
#include <format>

struct result_t
{
  int first;
  int second;
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

auto parse_file(const char* filename) -> std::vector<result_t>
{
  std::vector<result_t> results;

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

    auto space_pos = line.find(' ');
    if (space_pos != std::string_view::npos)
    {
      auto first_opt = parse_int(trim(line.substr(0, space_pos)));
      auto second_opt= parse_int(trim(line.substr(space_pos + 1)));
      if (!first_opt.has_value() || !second_opt.has_value())
      {
        abort();
      }
      results.push_back({first_opt.value(), second_opt.value()});
    }

    start = end + 1;
  }

  return results;
}

int count_occurances(int *sorted_begin, int *sorted_end, int value)
{
  auto begin = std::lower_bound(sorted_begin, sorted_end, value);
  auto end = std::upper_bound(sorted_begin, sorted_end, value);
  return end - begin;
}

int main()
{
  auto data = parse_file(CURRENT_SOURCE_DIR "/input");
  std::unique_ptr<int[]> first(new int[data.size()]);
  std::unique_ptr<int[]> second(new int[data.size()]);
  for (int i = 0; i < data.size(); i++)
  {
    first[i] = data[i].first;
    second[i] = data[i].second;
  }
  std::sort(first.get(), first.get() + data.size());
  std::sort(second.get(), second.get() + data.size());
  size_t acc = 0;
  for (int i = 0; i < data.size(); i++)
  {
    acc += std::abs(first[i] - second[i]);
  }

  std::printf("%s\n", std::format("Part one: {}", acc).c_str());

  acc = 0;
  for (int i = 0; i < data.size(); i++)
  {
    acc += count_occurances(second.get(), second.get() + data.size(), first[i]) * first[i];
  }

  std::printf("%s\n", std::format("Part two: {}", acc).c_str());
  return 0;
}
