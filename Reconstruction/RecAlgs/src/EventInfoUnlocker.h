/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECALGS_EVENTINFOUNLOCKER_H
#define RECALGS_EVENTINFOUNLOCKER_H

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

/** @class EventInfoUnlocker
    
To be scheduled when running reonstruction from ESD - copies EventInfo container content into new non-const container and overwrites StoreGate

@author Nick Styles <nicholas.styles AT cern.ch>
*/

class EventInfoUnlocker : public AthAlgorithm
{
 public:
  
  /** Standard Athena-Algorithm Constructor */
  EventInfoUnlocker(const std::string& name, ISvcLocator* pSvcLocator);
  
  /** Default Destructor */
  ~EventInfoUnlocker() {};
  
  /** standard Athena-Algorithm method */
  StatusCode          initialize();
  /** standard Athena-Algorithm method */
  StatusCode          execute();
  /** standard Athena-Algorithm method */
  StatusCode          finalize()
  { return StatusCode::SUCCESS; };
  
};

#endif 
