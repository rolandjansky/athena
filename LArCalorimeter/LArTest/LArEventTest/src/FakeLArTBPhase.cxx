/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArEventTest/FakeTBPhase.h"
#include "CxxUtils/make_unique.h"
#include "CLHEP/Units/SystemOfUnits.h"

using CLHEP::ns;

FakeTBPhase::FakeTBPhase(const std::string & name, ISvcLocator * pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
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
  m_phaseInd = (short)floor(m_phase/m_delta*(float)m_timeBins);
  ATH_MSG_INFO ( "Phase = " << m_phase << " - PhaseInd = " << m_phaseInd );
  return StatusCode::SUCCESS;
}

StatusCode FakeTBPhase::execute()
{
  ATH_MSG_DEBUG ( "Phase = " << m_phase );
  
  ATH_CHECK( evtStore()->record( CxxUtils::make_unique<TBPhase>
                                   (m_phase,m_phaseInd,0.),
                                 m_keyTBPhase) );
  return StatusCode::SUCCESS;
}

StatusCode FakeTBPhase::finalize()
{
  return StatusCode::SUCCESS;
}
