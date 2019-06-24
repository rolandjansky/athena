/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/OHLockedHist.h"
#include "TH1F.h"
#include <boost/thread/mutex.hpp>

void testTemplate()
{
  boost::mutex mutex;
  oh_lock_histogram_mutex::set_histogram_mutex(mutex);

  TH1F* h = new TH1F("h", "test h", 10, 0, 10);
  oh_lock_histogram<TH1F>(h)->Fill(0.5);

  oh_lock_histogram_mutex::reset_histogram_mutex();

  {
    oh_scoped_lock_histogram lock;
  }

  /* no dynamic allocation allowed */
  // new lock_histogram<TH1F>(h);

  /* no copy constructor */
  // lock_histogram<TH1F> l1(h); lock_histogram<TH1F> l2(l1);
}

int main()
{
  return 0;
}
