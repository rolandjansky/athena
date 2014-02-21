/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4FastSimulation/IFastSimDedicatedSD.h"
#include <iostream>
#include "LArG4Code/LArVHitMerger.h"
#include "LArG4Code/LArVHitMergerFactory.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"

IFastSimDedicatedSD::IFastSimDedicatedSD(const std::string & name):  FADS::FadsSensitiveDetector(name),m_hitMerger(0) {

#ifdef _TRACE_FADSD_
  msg(MSG::INFO) << "HELLO From " << GetName() << ", A Fast Sim Dedicated SD" << endreq;
#endif

  Register();
}

// Initialize each event.
void IFastSimDedicatedSD::Initialize(G4HCofThisEvent *) {

#ifdef _TRACE_FADSD_
  msg(MSG::INFO) << "INIT From " << GetName() << ", A Fast Sim Dedicated SD" << endreq;
#endif

  // At the beginning of first event initialize m_hitMerger pointer
  // 1. Get Hit Merger factory from DS
  // 2. Query Hit Merger factory for Hit Merger pointer
  if(m_hitMerger==0)
  {
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    StoreGateSvc* pSvc = 0;

    if(svcLocator->service("DetectorStore",pSvc)!=StatusCode::SUCCESS) 
      throw std::runtime_error ("IFastSimDedicatedSD::Initialize. Cannot get detector store");

    const DataHandle<LArVHitMergerFactory> _factory;
    if(pSvc->retrieve(_factory,"LArHitMergerFactory")==StatusCode::FAILURE || _factory==((LArVHitMergerFactory * ) 0))
      throw std::runtime_error ("IFastSimDedicatedSD::Initialize. Cannot retrieve hit merger factory");

    m_hitMerger = _factory->getHitMerger();

    if(m_hitMerger==0)
      throw std::runtime_error ("IFastSimDedicatedSD::Initialize. 0 pointer to the Hit Merger");
  }

  // Let Hit Merger prepare for the event
  m_hitMerger->BeginOfEvent();
}

// End each event (do hit merger here)
void IFastSimDedicatedSD::EndOfEvent(G4HCofThisEvent *) {

#ifdef _TRACE_FADSD_
  msg(MSG::INFO) << "EOEV From " << GetName() << ", A Fast Sim Dedicated SD" << endreq;
#endif

  if(m_hitMerger)
    m_hitMerger->EndOfEvent();

}

