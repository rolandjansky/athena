/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "EvgenProdTools/EvgenOTFTopUpSvc.h"

EvgenOTFTopUpSvc::EvgenOTFTopUpSvc(const std::string& name, ISvcLocator* pSvcLocator)
  : AthService(name, pSvcLocator)
  , m_nUsedSoFar(0)
  , m_newFileFlag(false)
  , m_nTotal(0)
  , m_nIterations(0)
{
  declareProperty("Efficiency",     m_efficiency=0.9,           "Estimate of Pythia/Herwig matching efficiency and filter efficiency");
  declareProperty("ExecDuringInit", m_execDuringInit=false,     "Execute OTF algorithm during intialize");
  declareProperty("NPerFile",       m_nPerFile=long(5000./0.9), "Number of events to make per file with OTF algorithm");
  declareProperty("NewFileName",    m_newFileName="events.lhe", "Please that the new OTF events went");
}

StatusCode EvgenOTFTopUpSvc::finalize() {
  ATH_MSG_INFO("Total on the fly generated events: " << m_nTotal << " over " << m_nIterations << " iterations.");
  return StatusCode::SUCCESS;
}

/** Query the interfaces - this is copied and pasted :( */
StatusCode EvgenOTFTopUpSvc::queryInterface(const InterfaceID& riid, void** ppvInterface){
  if ( IID_IEvgenOTFTopUpSvc == riid ) 
    *ppvInterface = (IEvgenOTFTopUpSvc*)this;
  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

#endif
