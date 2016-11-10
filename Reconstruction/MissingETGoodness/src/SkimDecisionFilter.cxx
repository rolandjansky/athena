/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MissingETGoodness/SkimDecisionFilter.h"
#include "EventBookkeeperMetaData/EventBookkeeperCollection.h"
#include "EventBookkeeperMetaData/SkimDecisionCollection.h"


// MB: reference to MET Goodies map for storing derived quantities
MET::Goodies& SkimDecisionFilter::s_goodies(MET::Goodies::instance());


SkimDecisionFilter::SkimDecisionFilter(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_metaStoreInInit(0)
{
  declareProperty( "ContainerName", m_containerName = "DESD_COLLCAND_SkimDecisionsContainer");
}


SkimDecisionFilter::~SkimDecisionFilter()
{ 
} 


StatusCode SkimDecisionFilter::initialize() 
{
  ATH_MSG_DEBUG ("initialize()");

  /** get a handle of StoreGate for access to the EventBookkeepers */
  ATH_CHECK( service("StoreGateSvc/InputMetaDataStore", m_metaStoreInInit) );

  /** Get EventBookkeepers */
  const EventBookkeeperCollection* coll = 0;
  StatusCode sc = m_metaStoreInInit->retrieve(coll, "EventBookkeepers");
  if (sc.isSuccess()) {
    EventBookkeeperCollection::const_iterator itr=coll->begin(), itrEnd = coll->end();
    for (; itr != itrEnd; itr++){
      uint64_t nEvt= (*itr)->getNAcceptedEvents();
      std::string name=(*itr)->getName();
      std::string description=(*itr)->getDescription();
      ATH_MSG_INFO ("Name=" << name << " : "<< description << " = " << nEvt);
    }
  } else {
    ATH_MSG_WARNING ("No EventBookkeeperCollection InInit was found: key = EventBookkeepers");
  }

  ATH_MSG_DEBUG ("initialize() successful");

  return StatusCode::SUCCESS;
}


StatusCode 
SkimDecisionFilter::execute() 
{
  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_DEBUG ("execute()");

  const SkimDecisionCollection *SDcoll = 0;

  if (evtStore()->contains<SkimDecisionCollection>(m_containerName)) {
    sc = evtStore()->retrieve(SDcoll, m_containerName);
    if ( sc.isSuccess() ) {
      SkimDecisionCollection::const_iterator itr=SDcoll->begin(), itrEnd = SDcoll->end();
      for(; itr != itrEnd; itr++){
        bool isAc = (*itr)->isAccepted();
        std::string name = (*itr)->getName();
        ATH_MSG_DEBUG ("SkimDecName = " << name <<" isAccepted: "<< isAc);
        s_goodies.setValue(name,static_cast<int>(isAc));
      }
    } else {
      ATH_MSG_WARNING ("Could not retrieve SkimDecisionCollection from StoreGate. key = " << m_containerName);
      return sc;
    }
  } else {
    ATH_MSG_DEBUG ("No SkimDecisionCollection was found: key = " << m_containerName);
    return sc;
  }

  ATH_MSG_DEBUG ("execute() successful");

  return StatusCode::SUCCESS;
}


StatusCode SkimDecisionFilter::finalize() 
{
  ATH_MSG_DEBUG ("finalize()");
  ATH_MSG_DEBUG ("finalize() successful");

  return StatusCode::SUCCESS;
}


