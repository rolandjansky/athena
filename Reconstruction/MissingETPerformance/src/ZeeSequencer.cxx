/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MissingETPerformance/ZeeSequencer.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include <algorithm>

//_________________________________________________________
ZeeSequencer::ZeeSequencer(const std::string& name,
								       ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_eventSelector ( "MissingETEventSelector" ) 
{

}
//_________________________________________________________
ZeeSequencer::~ZeeSequencer() {}
//_________________________________________________________
StatusCode ZeeSequencer::initialize()
{
  return StatusCode::SUCCESS;
}
//_________________________________________________________
StatusCode ZeeSequencer::finalize()
{
  return StatusCode::SUCCESS;
}
//_________________________________________________________
StatusCode ZeeSequencer::execute()
{
  // Retrive the containers for the EventSelector
  m_eventSelector->retrieveContainers();

  // Activate the sequence if passed
  setFilterPassed(m_eventSelector->isSelectedEvent() );
    
  return StatusCode::SUCCESS;
}
