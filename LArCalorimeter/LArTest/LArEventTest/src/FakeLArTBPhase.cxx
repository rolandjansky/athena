/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArEventTest/FakeTBPhase.h"
#include "CLHEP/Units/SystemOfUnits.h"

using CLHEP::ns;

FakeTBPhase::FakeTBPhase(const std::string & name, ISvcLocator * pSvcLocator)
  : Algorithm(name, pSvcLocator),
    m_StoreGate(0),
    m_phaseInd(0)
{
  declareProperty("TBPhase",        m_phase=25.);
  declareProperty("TBPhaseKey",     m_keyTBPhase="TBPhase");   
  declareProperty("TTCClockPeriod", m_delta=25.0*ns);
  declareProperty("TimeBins",       m_timeBins=25);
}

FakeTBPhase::~FakeTBPhase() {}

StatusCode FakeTBPhase::initialize() 
{
  MsgStream log(messageService(),name());
  StatusCode sc;

  sc = service( "StoreGateSvc", m_StoreGate);
  if( sc.isFailure() ) {
    log << MSG::FATAL << name() 
  	<< ": Unable to locate Service StoreGateSvc" << endreq;
    return sc;
  } 

  m_phaseInd = (short)floor(m_phase/m_delta*(float)m_timeBins);

  log << MSG::INFO << "Phase = " << m_phase << " - PhaseInd = " << m_phaseInd << endreq;

  return StatusCode::SUCCESS;
}

StatusCode FakeTBPhase::execute()
{
  MsgStream log(msgSvc(), name());
  StatusCode sc;

  log << MSG::DEBUG << "Phase = " << m_phase << endreq;
  
  TBPhase* m_TBPhase = new TBPhase(m_phase,m_phaseInd,0.);
  
  sc = m_StoreGate->record(m_TBPhase,m_keyTBPhase);
  if (sc.isFailure( )) {
    log << MSG::FATAL << "Cannot record fake TBPhase to StoreGate" << endreq;
    return StatusCode::FAILURE;
  }

return StatusCode::SUCCESS;
}

StatusCode FakeTBPhase::finalize()
{return StatusCode::SUCCESS;}
