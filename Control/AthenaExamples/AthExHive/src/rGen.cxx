/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "rGen.h"
#include <thread>

std::once_flag rGen::m_f;
std::default_random_engine rGen::m_gen;
std::uniform_real_distribution<float> rGen::m_dst(0.5,2.0);

void rGen::init() {
  unsigned long seed = std::hash<std::thread::id>()(std::this_thread::get_id());
  m_gen.seed(seed);
}
