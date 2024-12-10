#include <cstdio>
#include <string>
#include <tuple>
#include <format>
#include <array>
#include <vector>

bool is_move_possible(int pos, int width, int data_size, int target_pos)
{
  if (target_pos < 0 || target_pos >= data_size)
  {
    return false;
  }

  if (std::abs(pos - target_pos) == 1 && pos / width != target_pos / width)
  {
    return false;
  }

  return true;
}

enum class direction_t
{
  up,
  right,
  down,
  left,
  max
};

enum class loop_detection_t
{
  none,
  loop
};

std::pair<loop_detection_t, int> walk_guard(int width, const std::string &data, const std::array<int, static_cast<size_t>(direction_t::max)> &moves, int pos, direction_t direction, std::vector<uint8_t> &visited)
{
  int unique_steps = 1;
  auto max_steps = int(data.size() * 2);
  for (int i = 0; i < max_steps; i++)
  {
    auto target_pos = pos + moves[static_cast<size_t>(direction)];
    if (!is_move_possible(pos, width, int(data.size()), target_pos))
    {
      break;
    }
    if (data[target_pos] == '#')
    {
      direction = direction_t((static_cast<size_t>(direction) + 1) % static_cast<size_t>(direction_t::max));
      continue;
    }
    if (!visited[target_pos])
    {
      unique_steps++;
    }
    if (visited[target_pos] & uint8_t(1) << static_cast<size_t>(direction))
    {
      return {loop_detection_t::loop, unique_steps};
    }
    visited[target_pos] |= uint8_t(1) << static_cast<size_t>(direction);
    pos = target_pos;
  }
  return {loop_detection_t::none, unique_steps};
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
  content.erase(std::remove(content.begin(), content.end(), '\n'), content.end());

  size_t next_caret = content.find('^');
  if (next_caret == std::string::npos)
  {
    abort();
  }
  int starting_pos = int(next_caret);

  std::array<int, static_cast<size_t>(direction_t::max)> moves{};
  moves[0] = -width;
  moves[1] = 1;
  moves[2] = width;
  moves[3] = -1;

  std::vector<uint8_t> visited(content.size());

  auto walk_result = walk_guard(width, content, moves, starting_pos, direction_t::up, visited);
  if (walk_result.first == loop_detection_t::loop)
  {
    abort();
  }
  auto part_one = walk_result.second;
  auto original_visited = visited;

  auto part_two = 0;
  for (int i = 0; i < int(visited.size()); i++)
  {
    if (!original_visited[i])
    {
      continue;
    }
    std::fill(visited.begin(), visited.end(), 0);
    content[i] = '#';
    auto walk = walk_guard(width, content, moves, starting_pos, direction_t::up, visited);
    content[i] = ' ';
    if (walk.first == loop_detection_t::loop)
    {
      part_two++;
    }
  }

  return {part_one, part_two};
}

int main()
{
  auto result = parse_file(CURRENT_SOURCE_DIR "/input");
  std::printf("%s\n", std::format("moves found: {}, possible_loop_positions: {}", result.first, result.second).c_str());
  return 0;
}
