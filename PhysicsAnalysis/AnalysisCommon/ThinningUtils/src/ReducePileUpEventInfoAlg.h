///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef THINNINGUTILS_REDUCEPILEUPEVENTINFOALG_H
#define THINNINGUTILS_REDUCEPILEUPEVENTINFOALG_H

/**
   @class ReducePileUpEventInfoAlg
   @brief Algorithm to create a new EventInfo object (and record it into StoreGate) 
   from an existing PileUpEventInfo object and in the process stripping away all
   EventInfo information from pileup events (keeping only the hard scatter).

   @author Karsten Koeneke

*/

/******************************************************************************
Name:        ReducePileUpEventInfoAlg

Author:      Karsten Koeneke (CERN)
Created:     February 2013

Description: Algorithm to create a new EventInfo object (and record it into StoreGate) 
             from an existing PileUpEventInfo object and in the process stripping away all
             EventInfo information from pileup events (keeping only the hard scatter).
******************************************************************************/

// STL includes
#include <string>

// Framework includes
#include "AthenaBaseComps/AthAlgorithm.h"

// forward declarations
class EventInfo;



class ReducePileUpEventInfoAlg : public AthAlgorithm {

public: 
  /** Standard constructor */
  ReducePileUpEventInfoAlg(const std::string& name, ISvcLocator* pSvcLocator);

  /** Standard destructor */
  virtual ~ReducePileUpEventInfoAlg();
  
public:
  /** Gaudi Service Interface method implementations: initialize */
  StatusCode initialize();

  /** Gaudi Service Interface method implementations: execute */
  StatusCode execute();

  /** Gaudi Service Interface method implementations: finalize */
  StatusCode finalize();


private:

  /// Name of the new EventInfo object
  std::string m_eiKey;


  // Declare some counters and initialize them to zero
  /// Event counter
  unsigned long m_nEventsProcessed;

};

#endif
