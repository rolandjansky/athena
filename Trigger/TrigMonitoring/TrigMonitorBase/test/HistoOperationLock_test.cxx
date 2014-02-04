/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigMonitorBase/TrigLockedHist.h"
#include <TH1F.h>
#include <boost/thread/mutex.hpp>

void testTemplate() {
  boost::mutex mutex;
  lock_histogram_mutex::set_histogram_mutex(mutex);
  
  TH1F* h = new TH1F("h", "test h", 10, 0, 10);
  lock_histogram_operation<TH1F>(h)->Fill(0.5);

  lock_histogram_mutex::reset_histogram_mutex();

  /* no dynamic allocation allowed */
  //new lock_histogram_operation<TH1F>(h);
  
  /* no copy constructor */
  //lock_histogram_operation<TH1F> l1(h); lock_histogram_operation<TH1F> l2(l1);
}

int main() {
  return 0;
}
