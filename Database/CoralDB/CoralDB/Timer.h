/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Timer.h,v 1.1 2007-02-25 07:23:13 jb Exp $

#include <sys/time.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <iomanip>


using namespace std;

class Timer {

 public:
  Timer(const std::string& description = "Timer", double n = 1.0)
    : m_description (description),
      m_n (n)
  { stop(); start(); }
  void start() { gettimeofday(&m_t0, NULL); }
  void stop() { gettimeofday(&m_t1, NULL); }
  double elapsed() { return m_t1.tv_sec - m_t0.tv_sec + 1.0e-6 * (m_t1.tv_usec - m_t0.tv_usec); }
  void print() { cout << left << setw(30) << m_description+":" << elapsed()/m_n*1000. << " ms" << endl; }

 private:
  string m_description;
  double m_n;
  struct timeval m_t0;
  struct timeval m_t1;
};
