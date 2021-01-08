/*                                                                                                                      
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration                                               
*/

// First the corresponding header.
#include "TrigT1MuctpiPhase1/SimController.h"
#include "TrigT1MuctpiPhase1/MuonSectorProcessor.h"
#include "TrigT1MuctpiPhase1/TriggerProcessor.h"


// The headers from other ATLAS packages,
// from most to least dependent.
#include "TrigT1Interfaces/Lvl1MuCTPIInputPhase1.h"
#include "TrigT1Interfaces/MuCTPIL1Topo.h"

// Headers from external packages.


// System headers.
#include <iostream>
#include <vector>

namespace LVL1MUCTPIPHASE1 {

  SimController::SimController() :
    m_triggerProcessor(new TriggerProcessor())
  {
    m_muonSectorProcessors.push_back(new MuonSectorProcessor(LVL1MUONIF::Lvl1MuCTPIInputPhase1::idSideA()));
    m_muonSectorProcessors.push_back(new MuonSectorProcessor(LVL1MUONIF::Lvl1MuCTPIInputPhase1::idSideC()));
  }
  
  SimController::~SimController()
  {
    for (int i=0;i<(int)m_muonSectorProcessors.size();i++) delete m_muonSectorProcessors[i];
    delete m_triggerProcessor;
  }

  void SimController::configureTopo(const std::string& geoFile)
  {
    for (int i=0;i<(int)m_muonSectorProcessors.size();i++) m_muonSectorProcessors[i]->configureTopo(geoFile);
  }

  void SimController::configureOverlapRemoval(const std::string& lutFile)
  {
    for (int i=0;i<(int)m_muonSectorProcessors.size();i++) m_muonSectorProcessors[i]->configureOverlapRemoval(lutFile);
  }

  // set Configuration                                                                                                                                      
  void SimController::setConfiguration ( const Configuration &conf ) {

    // Zero Suppression Flag                                                                                                                               
    m_doZeroSuppression = conf.getDoZeroSuppression();

    //src/Mirod/MirodExtractor.cxx
    m_threshold1Candidate = conf.getThreshold1Candidate();
    m_threshold2Candidate = conf.getThreshold2Candidate();
    m_suppressionMode = conf.getSuppressionMode();


    //MirodLvl2Processor
    m_maxCandPerPtvalue = conf.getMaxCandPerPtvalue();
    m_maxCandSendToRoib = conf.getMaxCandSendToRoib();
    m_candBcidOffset = conf.getCandBcidOffset();
  }

  void SimController::processData(LVL1MUONIF::Lvl1MuCTPIInputPhase1* input, int bcid)
  {
    std::vector<LVL1MUONIF::Lvl1MuCTPIInputPhase1*> processedInputs;
    int nMSP = m_muonSectorProcessors.size();
    for (int i=0;i<nMSP;i++)
    {
      m_muonSectorProcessors[i]->setInput(input);
      m_muonSectorProcessors[i]->runOverlapRemoval();
      m_muonSectorProcessors[i]->makeTriggerObjectSelections();
      m_muonSectorProcessors[i]->makeL1TopoData();
      processedInputs.push_back(m_muonSectorProcessors[i]->getOutput());
    }

    //Run the trigger processor algorithms
    
    m_triggerProcessor->mergeInputs(processedInputs);
    m_triggerProcessor->computeMultiplicities(bcid);
    m_triggerProcessor->makeTopoSelections();
  }

  LVL1::MuCTPIL1Topo SimController::getL1TopoData(int bcidOffset)
  {
    LVL1::MuCTPIL1Topo l1topo;
    int nMSP = m_muonSectorProcessors.size();
    for (int i=0;i<nMSP;i++)
    {
      l1topo += m_muonSectorProcessors[i]->getL1TopoData(bcidOffset);
    }
    return l1topo;
  }

  TriggerProcessor* SimController::getTriggerProcessor()
  {
    return m_triggerProcessor;
  }

}

