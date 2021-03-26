///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// AthEventCounter.h
// Header file for class AthEventCounter
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef GAUDISEQUENCER_ATHEVENTCOUNTER_H
#define GAUDISEQUENCER_ATHEVENTCOUNTER_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "Gaudi/Property.h"
#include <atomic>

class AthEventCounter : public AthReentrantAlgorithm 
{
public:

  using AthReentrantAlgorithm::AthReentrantAlgorithm;
  ~AthEventCounter( ) = default;

  /*****************************
   ** Public Function Members **
   *****************************/

  StatusCode initialize();
  StatusCode execute(const EventContext& ctx) const ;
  StatusCode finalize();

private:

  /**************************
   ** Private Data Members **
   **************************/

  /**
   ** The frequency with which the number of events
   ** should be reported. The default is 1, corresponding
   ** to every event.
   **/
  Gaudi::Property<int> m_frequency{this, "Frequency", 1,
      "The frequency with which the number of events should be "
      "reported. The default is 1, corresponding to every event" };

  /**
   ** The total events seen.
   **/
  mutable std::atomic<int> m_total{0};
};

#endif // GAUDISEQUENCER_ATHEVENTCOUNTER_H

