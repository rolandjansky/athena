#include <bitset>
unsigned int CountBits(int data)
{
  std::bitset<4> bs(data);
  return bs.count();
}
