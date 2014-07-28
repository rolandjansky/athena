/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTDIGSIMPLETIMER_H
#define TRTDIGSIMPLETIMER_H

#include <ctime>

/**
 * For timing of code
 *
 * Timer needs to be explicitly started using 'start' or 'restart'.
 * It can be stopped and started again, and "check" will then report
 * the total accumulated time.
 */
class TRTDigSimpleTimer {
 public:
  TRTDigSimpleTimer();  /**< Constructor */

  void start();         /**< Starts the timer again          */
  void restart();       /**< Also resets accumulated time    */
  void stop();          /**< Does not reset accumulated time */
  double check();       /**< Report aggregate time           */

 private:
  int running;
  clock_t start_clock;
  time_t start_time;
  double acc_time;

  double elapsed_time();

};

#endif

