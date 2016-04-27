/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef hltca_HLTCountOHReceiver
#define hltca_HLTCountOHReceiver

// This macro interferes with some tdaq headers:
#undef PACKAGE_VERSION

#include "HLTRates2COOL/HLTCounter.h"

#include <iostream>
#include <vector>

#include <owl/regexp.h>

// OH include files
#include <oh/OHRootReceiver.h>
#include <oh/OHSubscriber.h>

#include "TH2.h"


ERS_DECLARE_ISSUE( hltca,
                   HLTCountReceivingIssue,
                   "HLTCountOHReceiver: " << explanation,
                   ((const char*) explanation)
                   )
	 

namespace hltca {

   class HLTCountOHReceiver : public OHRootReceiver {
   public:

      HLTCountOHReceiver();    

      using OHRootReceiver::receive;

      void receive( OHRootHistogram & h);

      void receive( std::vector<OHRootHistogram*> & )
      {
         std::cout << "receive( std::vector<OHRootHistogram*> & ) notification has been received" << std::endl;
      }

      int last_LB() const;
      
      const std::vector<hltca::HLTCounter>& hltCounters(int lumiblock, hltca::HLTCounter::TriggerLevel level);

      void ClearStorage();
      
   private:

      struct Content {
         Content(): h(0), valid(true), filled(false), sum(0) {}
         ~Content() {delete h;}
         TH2I * h;
         std::vector<HLTCounter> counters;
         bool valid; // true: content of h agrees with content of counters
         bool filled; // true after the first histogram for is received
         int sum;
      };

      void updateInternalStore(const OHRootHistogram & h);

      void CopyHistContent(const TH2& src, TH2& dest);

      void SetChainCounterBins(TAxis *axis, int level);

      void updateHLTCounters(Content& c, HLTCounter::TriggerLevel level);

      std::vector<Content> fCountStore[2];     // one store for each level
      std::vector<int>     fChainCounters[2];  // bin index for chain_counter


   }; //end class HLTCountOHReceiver

}

#endif
