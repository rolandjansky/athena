///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthEventCounter.h
// Header file for class AthEventCounter
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef GAUDISEQUENCER_ATHEVENTCOUNTER_H
#define GAUDISEQUENCER_ATHEVENTCOUNTER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "Gaudi/Property.h"

class AthEventCounter : public AthAlgorithm 
{
public:

  /**
   ** Constructor(s)
   **/
  AthEventCounter( const std::string& name, ISvcLocator* pSvcLocator );

  /**
   ** Destructor
   **/
  ~AthEventCounter( );

  /*****************************
   ** Public Function Members **
   *****************************/

  StatusCode initialize();
  StatusCode execute();
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
  Gaudi::CheckedProperty<int> m_frequency;

  /**
   ** The number of events skipped since the last time
   ** the count was reported.
   **/
  int m_skip;

  /**
   ** The total events seen.
   **/
  int m_total;
};

#endif // GAUDISEQUENCER_ATHEVENTCOUNTER_H

