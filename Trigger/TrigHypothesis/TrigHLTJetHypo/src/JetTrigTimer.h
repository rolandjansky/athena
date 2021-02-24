/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_JETTRIGTIMER_H
#define TRIGHLTJETHYPO_JETTRIGTIMER_H

#include <chrono>
#include <vector>
#include <string>
#include <tuple>

class JetTrigTimer{
 public:
  JetTrigTimer(bool nanoseconds=false);
  
  void start();
  void update();
  std::string read() const;
  std::tuple<double, int, std::string> read_bare() const;
  void reset() noexcept;
  std::string readAndReset();
  double elapsed_to_now();
  double elapsed_to_update();
  std::string units() const;
 
 private:
  std::size_t m_nCalls{0};
  std::chrono::system_clock::time_point m_start;
  std::chrono::system_clock::time_point m_stop;

  double m_elapsedDelta{0.};
  bool m_nanoseconds;
  void accumulate();

};
#endif

