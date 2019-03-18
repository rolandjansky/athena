///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// EventCounterAlg.h
// Header file for class EventCounterAlg
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef EVENTBOOKKEEPERTOOLS_EVENTCOUNTERALG_H
#define EVENTBOOKKEEPERTOOLS_EVENTCOUNTERALG_H 1

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "AthenaBaseComps/AthFilterAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"


class EventCounterAlg
  : public ::AthFilterAlgorithm
{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:

  // Copy constructor:

  /// Constructor with parameters:
  EventCounterAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor:
  virtual ~EventCounterAlg();

  // Assignment operator:
  //EventCounterAlg &operator=(const EventCounterAlg &alg);

  // Athena algorithm's Hooks
  virtual StatusCode  initialize() override;
  virtual StatusCode  execute() override;
  virtual StatusCode  finalize() override;

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
 private:

  /// Default constructor:
  EventCounterAlg();

  /// Property to set if all MC event-weights, including the non-nominal ones, should be tracked
  bool m_trackOtherMCWeights;

  /// An event counter
  unsigned long m_eventsProcessed;

  /// Keep a vector of all cutIDs for the non-nominal MC event weights
  std::vector<CutIdentifier> m_mcCutIDs;
};

// I/O operators
//////////////////////

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////


#endif //> !EVENTBOOKKEEPERTOOLS_EVENTCOUNTERALG_H
