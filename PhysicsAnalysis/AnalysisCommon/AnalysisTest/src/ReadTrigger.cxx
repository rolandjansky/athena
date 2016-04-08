/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisTest/ReadTrigger.h"

#include "AnalysisTriggerEvent/CTP_Decision.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

ReadTrigger::ReadTrigger(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadTrigger::initialize()
{
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadTrigger::execute()
{
  ATH_MSG_DEBUG ("in execute()");

  // Retrieve CTP
  const CTP_Decision * ctpDecision = 0;
  ATH_CHECK( evtStore()->retrieve(ctpDecision) );

  ATH_MSG_DEBUG ("word0 : " << MSG::hex << ctpDecision->getWord0());
  ATH_MSG_DEBUG ("word1 : " << MSG::hex << ctpDecision->getWord1());
  ATH_MSG_DEBUG ("word2 : " << MSG::hex << ctpDecision->getWord2());
  ATH_MSG_DEBUG ("word3 : " << MSG::hex << ctpDecision->getWord3());
  ATH_MSG_DEBUG ("word4 : " << MSG::hex << ctpDecision->getWord4());
  ATH_MSG_DEBUG ("word5 : " << MSG::hex << ctpDecision->getWord5());
  ATH_MSG_DEBUG ("word6 : " << MSG::hex << ctpDecision->getWord6());
  ATH_MSG_DEBUG ("word7 : " << MSG::hex << ctpDecision->getWord7());

  for (const auto& item : ctpDecision->getItems())
    {
      ATH_MSG_DEBUG ("item : " << item);
    }

  // Retrieve LVL1 ROI
  const LVL1_ROI* roi = 0;
  ATH_CHECK( evtStore()->retrieve(roi) );

  for (const auto& item : roi->getMuonROIs())
    {
      ATH_MSG_DEBUG ("Mu : " << item.getEta() << " " << item.getPhi());
    }

  for (const auto& item : roi->getEmTauROIs())
    {
      ATH_MSG_DEBUG ("EmTau : " << item.getEta() << " " << item.getPhi());
    }

  for (const auto& item : roi->getJetROIs())
    {
      ATH_MSG_DEBUG ("Jet : " << item.getEta() << " " << item.getPhi());
    }
 
  for (const auto& item : roi->getJetEtROIs())
    {
      ATH_MSG_DEBUG ("JetET : " << MSG::hex << item.getROIWord() << MSG::dec);
    }
 
  for (const auto& item : roi->getEnergySumROIs())
    {
      ATH_MSG_DEBUG ("EnergySum : " << item.getEnergyX() << " "
                     << item.getEnergyY() << " " << item.getEnergyT());
    }
 
 return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadTrigger::finalize()
{
  return StatusCode::SUCCESS;
}
