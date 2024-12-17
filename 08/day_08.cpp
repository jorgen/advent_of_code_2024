#include <cstdio>
#include <string>
#include <unordered_map>
#include <format>
#include <vector>
#include <algorithm>

bool check_anti_node_bounds(int anti_node, int data_length)
{
  return anti_node >= 0 && anti_node < data_length;
}

template<bool increasing>
bool is_valid_anti_node(int anti_node, int antenna_line, int line_diff,  int width, int data_length)
{
  if (!check_anti_node_bounds(anti_node, data_length))
  {
    return false;
  }
  auto anti_node_line = anti_node / width;
  if (increasing)
  {
    if (anti_node_line - antenna_line != line_diff)
    {
      return false;
    }
  }
  else
  {
    if (antenna_line - anti_node_line != line_diff)
    {
      return false;
    }
  }
  return true;
}

void add_anti_nodes_pair(const std::pair<int, int> &antenna_pair, int width, int data_length, std::vector<int> &anti_nodes)
{
  int diff = antenna_pair.second - antenna_pair.first;
  int first_antenna_line = antenna_pair.first / width;
  int second_antenna_line = antenna_pair.second / width;
  int line_diff = second_antenna_line - first_antenna_line;

  auto first_anti_node = antenna_pair.first - diff;
  if (is_valid_anti_node<false>(first_anti_node, first_antenna_line, line_diff, width, data_length))
  {
    anti_nodes.emplace_back(first_anti_node);
  }
  auto second_anti_node = antenna_pair.second + diff;
  if (is_valid_anti_node<true>(second_anti_node, second_antenna_line, line_diff, width, data_length))
  {
    anti_nodes.emplace_back(second_anti_node);
  }
}

void add_anti_nodes_for_antenna_pair(const std::pair<int, int> &antenna_pair, int width, int data_length, std::vector<int> &anti_nodes)
{
  if (antenna_pair.second < antenna_pair.first)
  {
    abort();
  }
  int diff = antenna_pair.second - antenna_pair.first;
  int first_antenna_line = antenna_pair.first / width;
  int second_antenna_line = antenna_pair.second / width;
  int line_diff = second_antenna_line - first_antenna_line;

  int previous_anti_node_line = first_antenna_line;
  int next_anti_node = antenna_pair.first - diff;

  while (is_valid_anti_node<false>(next_anti_node, previous_anti_node_line, line_diff, width, data_length))
  {
    anti_nodes.emplace_back(next_anti_node);
    previous_anti_node_line = next_anti_node / width;
    next_anti_node -= diff;
  }

  previous_anti_node_line = second_antenna_line;
  next_anti_node = antenna_pair.second + diff;
  while (is_valid_anti_node<true>(next_anti_node, previous_anti_node_line, line_diff, width, data_length))
  {
    anti_nodes.emplace_back(next_anti_node);
    previous_anti_node_line = next_anti_node / width;
    next_anti_node += diff;
  }
}

auto parse_file(const char* filename) -> std::pair<int, int>
{
  std::FILE* file = std::fopen(filename, "r");
  if (!file) abort();

  std::fseek(file, 0, SEEK_END);
  auto size = std::ftell(file);
  std::rewind(file);

  std::string content(size, '\0');
  std::fread(content.data(), 1, size, file);
  std::fclose(file);

  size_t next_newline = content.find('\n');
  if (next_newline== std::string::npos)
  {
    abort();
  }
  int width = int(next_newline);
  std::unordered_map<char, std::vector<int>> antennas;
  int antenna_count = 0;
  int new_lines = 0;
  for (int i = 0; i < int(content.size()); i++)
  {
    if (content[i] == '.')
    {
      continue;
    }
    if (content[i] == '\n')
    {
      new_lines++;
      continue;
    }
    antennas[content[i]].emplace_back(i - new_lines);
    antenna_count++;
  }

  auto content_length = int(content.size() - new_lines);
  std::vector<int> anti_nodes;
  anti_nodes.reserve(antenna_count);

  std::vector<int> anti_nodes_harmonics;
  anti_nodes_harmonics.reserve(antenna_count * 3);

  for (auto &[name, positions] : antennas)
  {
    (void)name;
    for (int m = 0; m < int(positions.size()); m++)
    {
      anti_nodes_harmonics.emplace_back(positions[m]);
      for (int n = m + 1; n < int(positions.size()); n++)
      {
        auto antenna_pair = std::make_pair(positions[m], positions[n]);

        //part one
        add_anti_nodes_pair(antenna_pair, width, content_length, anti_nodes);

        //part two
        add_anti_nodes_for_antenna_pair(antenna_pair, width, content_length, anti_nodes_harmonics);
      }
    }
  }
  std::ranges::sort(anti_nodes);
  auto to_remove = std::ranges::unique(anti_nodes);
  anti_nodes.erase(to_remove.begin(), to_remove.end());

  auto part_one = int(anti_nodes.size());

  std::ranges::sort(anti_nodes_harmonics);
  to_remove = std::ranges::unique(anti_nodes_harmonics);
  auto anti_nodes_size = std::distance(anti_nodes_harmonics.begin(), to_remove.begin());
  (void) anti_nodes_size;
  anti_nodes_harmonics.erase(to_remove.begin(), to_remove.end());

  auto part_two = int(anti_nodes_harmonics.size());

  return {part_one, part_two};
}

int main()
{
  auto result = parse_file(CURRENT_SOURCE_DIR "/input");
  std::printf("%s\n", std::format("part one: {}, part_two: {}", result.first, result.second).c_str());
  return 0;
}
