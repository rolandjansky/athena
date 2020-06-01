/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */

#undef NDEBUG
#include "AthenaKernel/AlgorithmTimer.h"
#include <cassert>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

#define PRINT_ME {std::cout << __FUNCTION__ << std::endl;}

using Athena::AlgorithmTimer;

constexpr size_t numTimers = 1000; // Extreme value for testing
constexpr size_t testTimeMilliSec = 100;
constexpr size_t callbackSleepMilliSec = 1;
std::atomic_size_t counter;
std::atomic_bool allowCallbacks{false}; // Ensures no callbacks are executed after timer deletion


/// Create a vector of timers for testing
std::vector<std::unique_ptr<AlgorithmTimer>> createTimers(const std::size_t n,
                                                          const std::size_t t,
                                                          const AlgorithmTimer::callbackFct_t& f) {
  std::vector<std::unique_ptr<AlgorithmTimer>> timers;
  for (size_t i=0; i<n; ++i)
    timers.push_back(std::make_unique<AlgorithmTimer>(t, f));
  return timers;
}

/// Callback sleeping for some time and incrementing the counter
void testCallback() {
  assert(allowCallbacks);
  std::this_thread::sleep_for(std::chrono::milliseconds(callbackSleepMilliSec));
  ++counter;
}

/// Test the standard timer functionality making sure N timers result in the callback
/// being executed N times within the expected time plus a fixed margin
void test1() {
  PRINT_ME
  counter = 0;
  const auto timers = createTimers(numTimers, testTimeMilliSec, &testCallback);
  allowCallbacks = true;
  std::this_thread::sleep_for(std::chrono::milliseconds(testTimeMilliSec + numTimers*callbackSleepMilliSec + 50));
  allowCallbacks = false;
  assert(counter == numTimers);
}

/// Test the case when timers are deleted before the scheduled time (no callback)
void test2() {
  PRINT_ME
  counter = 0;
  allowCallbacks = false;
  auto timers = createTimers(numTimers, testTimeMilliSec, &testCallback);
  std::this_thread::sleep_for(std::chrono::milliseconds(testTimeMilliSec/2));
  for (auto& ptr : timers) ptr.reset();
  // wait until the scheduled time passes to make sure no callbacks were executed
  std::this_thread::sleep_for(std::chrono::milliseconds(testTimeMilliSec/2 + 50));
  assert(counter == 0);
}

/// Test potential race condition when timers are deleted at the same time as callbacks are called
void test3() {
  PRINT_ME
  counter = 0;
  auto timers = createTimers(numTimers, testTimeMilliSec, &testCallback);
  allowCallbacks = true;
  std::this_thread::sleep_for(std::chrono::milliseconds(testTimeMilliSec));
  for (auto& ptr : timers) ptr.reset();
  // ensure callback assertion fails if callback is executed after timer is deleted
  allowCallbacks = false;
  // no assertion on counter value, which has an undefined value here
}

int main() {
  test1();
  test2();
  test3();
  return 0;
}