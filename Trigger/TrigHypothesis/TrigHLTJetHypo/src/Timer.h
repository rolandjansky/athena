/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_TIMER_H
#define TRIGHLTJETHYPO_TIMER_H

#include <chrono>
#include <vector>
#include <string>

using namespace std::chrono;
class JetTrigTimer{
 public:
  void start();
  void stop();
  std::string readAndReset();
  void reset() noexcept;
 private:
  system_clock::time_point m_start;
  system_clock::time_point m_stop;
  bool m_isRunning{false};
  double m_delta{0.};
  std::size_t m_nCalls{0};
};
#endif
