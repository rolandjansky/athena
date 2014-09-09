/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandomEngine.h"
#include "TestRandomSeqAlg.h"
TestRandomSeqAlg::TestRandomSeqAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_rndmSvc("AtRanluxGenSvc", name),
  m_streamName("TEST"),
  m_noOfNo(10),
  m_pEng(0)
{
  declareProperty("RndmSvc", m_rndmSvc, "the IAtRndmGenSvc we want to test (default AtRanluxGenSvc");
  declareProperty("StreamName", m_streamName, " random number stream to use (default TEST");
  declareProperty("NoOfNo", m_noOfNo, "the number of random numbers to shoot and print per event (default 10)");
}

StatusCode
TestRandomSeqAlg::initialize() {
  msg() << MSG::INFO << " Initializing " << endmsg;
  return ((m_rndmSvc.retrieve()).isSuccess() && 
	  (m_pEng = m_rndmSvc->GetEngine(m_streamName.value()))) ?
    StatusCode::SUCCESS :
    StatusCode::FAILURE;
}

StatusCode
TestRandomSeqAlg::execute() {
  msg() << MSG::DEBUG << "execute: random sequence: ";
  for (int i=0; i<m_noOfNo.value(); ++i) msg() << m_pEng->flat() << " ";
  msg() << endmsg;
  return StatusCode::SUCCESS;
}
