/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisTest/ReadTrigger.h"

#include "StoreGate/StoreGate.h"
#include "GaudiKernel/MsgStream.h"
#include "AnalysisTriggerEvent/CTP_Decision.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

ReadTrigger::ReadTrigger(const std::string& name, ISvcLocator* pSvcLocator)
  : Algorithm(name, pSvcLocator)
{}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadTrigger::initialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in initialize()" << endreq;

  StatusCode sc;

  // get StoreGate service
  sc = service("StoreGateSvc",m_storeGate);
  if (sc.isFailure())
    {
      log << MSG::FATAL << "StoreGate service not found !" << endreq;
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadTrigger::execute()
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in execute()" << endreq;

  StatusCode sc;

  // Retrieve CTP
  const CTP_Decision * ctpDecision;
  sc =m_storeGate->retrieve(ctpDecision);
  if (sc.isFailure())
    {
      log << MSG::FATAL << "CTP_Decision could not be retrieved from StoreGate !" << endreq;
      return StatusCode::FAILURE;
    }

  log << MSG::DEBUG << "word0 : " << MSG::hex << ctpDecision->getWord0() << endreq;
  log << MSG::DEBUG << "word1 : " << MSG::hex << ctpDecision->getWord1() << endreq;
  log << MSG::DEBUG << "word2 : " << MSG::hex << ctpDecision->getWord2() << endreq;
  log << MSG::DEBUG << "word3 : " << MSG::hex << ctpDecision->getWord3() << endreq;
  log << MSG::DEBUG << "word4 : " << MSG::hex << ctpDecision->getWord4() << endreq;
  log << MSG::DEBUG << "word5 : " << MSG::hex << ctpDecision->getWord5() << endreq;
  log << MSG::DEBUG << "word6 : " << MSG::hex << ctpDecision->getWord6() << endreq;
  log << MSG::DEBUG << "word7 : " << MSG::hex << ctpDecision->getWord7() << MSG::dec << endreq;

  CTP_Decision::items_type::const_iterator itCTP  = (ctpDecision->getItems()).begin();
  CTP_Decision::items_type::const_iterator itCTPe = (ctpDecision->getItems()).end();
  for (; itCTP != itCTPe; ++itCTP)
    {
      log << MSG::DEBUG << "item : " << *itCTP << endreq;
    }

  // Retrieve LVL1 ROI
  const LVL1_ROI * roi;
  sc =m_storeGate->retrieve(roi);
  if (sc.isFailure())
    {
      log << MSG::FATAL << "LVL1_ROI could not be retrieved from StoreGate !" << endreq;
      return StatusCode::FAILURE;
    }

  LVL1_ROI::muons_type::const_iterator itMU  = (roi->getMuonROIs()).begin();
  LVL1_ROI::muons_type::const_iterator itMUe = (roi->getMuonROIs()).end();
  for (; itMU != itMUe; ++itMU)
    {
      log << MSG::DEBUG << "Mu : " << itMU->getEta() << " " << itMU->getPhi() << endreq;
    }

  LVL1_ROI::emtaus_type::const_iterator itEM  = (roi->getEmTauROIs()).begin();
  LVL1_ROI::emtaus_type::const_iterator itEMe = (roi->getEmTauROIs()).end();
  for (; itEM != itEMe; ++itEM)
    {
      log << MSG::DEBUG << "EmTau : " << itEM->getEta() << " " << itEM->getPhi() << endreq;
    }

  LVL1_ROI::jets_type::const_iterator itJET  = (roi->getJetROIs()).begin();
  LVL1_ROI::jets_type::const_iterator itJETe = (roi->getJetROIs()).end();
  for (; itJET != itJETe; ++itJET)
    {
      log << MSG::DEBUG << "Jet : " << itJET->getEta() << " " << itJET->getPhi() << endreq;
    }
 
  LVL1_ROI::jetets_type::const_iterator itJetET  = (roi->getJetEtROIs()).begin();
  LVL1_ROI::jetets_type::const_iterator itJetETe = (roi->getJetEtROIs()).end();
  for (; itJetET != itJetETe; ++itJetET)
    {
      log << MSG::DEBUG << "JetET : " << MSG::hex << itJetET->getROIWord() << MSG::dec << endreq;
    }
 
  LVL1_ROI::energysums_type::const_iterator itES  = (roi->getEnergySumROIs()).begin();
  LVL1_ROI::energysums_type::const_iterator itESe = (roi->getEnergySumROIs()).end();
  for (; itES != itESe; ++itES)
    {
      log << MSG::DEBUG << "EnergySum : " << itES->getEnergyX() << " "
	  << itES->getEnergyY() << " " << itES->getEnergyT() << endreq;
    }
 
 return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadTrigger::finalize()
{
  return StatusCode::SUCCESS;
}
