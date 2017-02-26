#include "rGen.h"
#include <iostream>

std::once_flag rGen::m_f;
std::default_random_engine rGen::m_gen;
std::uniform_real_distribution<float> rGen::m_dst(0.5,2.0);

void rGen::init() {
  unsigned long seed = std::hash<std::thread::id>()(std::this_thread::get_id());
  // std::cout << "0----> rGen::init with seed " << seed << std::endl;
  m_gen.seed(seed);
}
