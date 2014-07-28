/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTDigSimpleTimer.h"

#include <ctime>

// __________________________________________________________________________
TRTDigSimpleTimer::TRTDigSimpleTimer() : running(0), start_clock(0), start_time(0), acc_time(0) {}

// __________________________________________________________________________
double TRTDigSimpleTimer::elapsed_time() {
  // Return the time that has passed since the timer was started last
  // time (not accumulated).
  return (clock() - start_clock) / static_cast<double>(CLOCKS_PER_SEC);
}

// __________________________________________________________________________
void TRTDigSimpleTimer::start() {
  // Start a timer.  If it is already running, let it continue running.

  // Return immediately if the timer is already running
  if (running)
    return;

  // Change timer status to running
  running = 1;

  // Set the start time;
  start_clock = clock();
  start_time = time(0);

}

// __________________________________________________________________________
void TRTDigSimpleTimer::restart() {
  // Turn the timer off and start it again from 0.

  // Set the timer status to running
  running = 1;

  // Set the accumulated time to 0 and the start time to now
  acc_time = 0;
  start_clock = clock();
  start_time = time(0);

}

// __________________________________________________________________________
void TRTDigSimpleTimer::stop()
{
  // Recalculate and store the total accumulated time up until now
  if (running)
    acc_time += elapsed_time();

  running = 0;
}

// __________________________________________________________________________
double TRTDigSimpleTimer::check() {
  return running ? acc_time + elapsed_time() : acc_time;
}
