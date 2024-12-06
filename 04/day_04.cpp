#include <cstdio>
#include <string>
#include <tuple>
#include <format>
#include <array>

bool is_move_possible(int pos, int width, int data_size, int target_pos)
{
  if (target_pos < 0 || target_pos >= data_size)
  {
    return false;
  }

  int current_line = pos / width;
  int target_line = target_pos / width;
  if (std::abs(current_line - target_line) > 1)
  {
    return false;
  }
  return true;
}

bool validate_mas_for_move(int pos, int width, const std::string &data, int move)
{
  std::string mas = "MAS";
  for (char ma : mas)
  {
    int target_pos = pos + move;
    if (!is_move_possible(pos, width, int(data.size()), target_pos))
    {
      return false;
    }
    if (data[target_pos] != ma)
    {
      return false;
    }
    pos = target_pos;
  }
  return true;
}

bool move_sequence_match_mas_permutation(int pos, int width, const std::string &data, const std::string_view &xmas_permutation, const std::array<int, 2> &moves)
{
    for (int i = 0; i < int(xmas_permutation.size()); i++)
    {
      auto target_pos = pos + moves[i];
      if (!is_move_possible(pos, width, int(data.size()), target_pos))
      {
        return false;
      }
      if (data[target_pos] != xmas_permutation[i])
      {
        return false;
      }
    }
    return true;
}

bool validate_cross_mas_for_pos(int pos, int width, const std::string &data)
{
  std::array<std::string, 2> x_data = {"MS", "SM"};
  std::array<int, 2> moves[2];
  moves[0][0] = -width - 1; moves[0][1] = width + 1;
  moves[1][0] = width - 1; moves[1][1] = -width + 1;

  for (auto &move : moves)
  {
    bool found = false;
    for (auto &x_mas_permutation : x_data)
    {
      if (move_sequence_match_mas_permutation(pos, width, data, x_mas_permutation, move))
      {
        found = true;
        break;
      }
    }
    if (!found)
    {
      return false;
    }
  }

  return true;
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
  int width = int(next_newline + 1);

  std::array<int, 8> moves{};
  moves[0] = 1;
  moves[1] = -1;
  moves[2] = width;
  moves[3] = -width;
  moves[4] = width + 1;
  moves[5] = width - 1;
  moves[6] = -width + 1;
  moves[7] = -width - 1;

  int x_mas_found = 0;
  int mas_in_cross_found = 0;
  for (int i = 0; i < int(content.size()); i++)
  {
    if (content[i] == 'X')
    {
      for (int move : moves)
      {
        if (validate_mas_for_move(i, width, content, move))
        {
          x_mas_found++;
          break;
        }
      }
    }
    if (content[i] == 'A')
    {
      if (validate_cross_mas_for_pos(i, width, content)) {
        mas_in_cross_found++;
      }
    }
  }

  return {x_mas_found, mas_in_cross_found};
}

int main()
{
  auto result = parse_file(CURRENT_SOURCE_DIR "/input");
  std::printf("xmas_found: %d\ncross_mas_found: %d\n", result.first, result.second);
  return 0;
}
