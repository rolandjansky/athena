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

  void SimController::configureTopo(const std::string& barrelFileName,
				    const std::string& ecfFileName,
				    const std::string& side0LUTFileName,
				    const std::string& side1LUTFileName)
  {
    m_l1topoLUT.initializeLUT(barrelFileName, ecfFileName, side0LUTFileName, side1LUTFileName);
    for (int i=0;i<(int)m_muonSectorProcessors.size();i++) m_muonSectorProcessors[i]->setL1TopoLUT(&m_l1topoLUT);
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

  bool SimController::processData(LVL1MUONIF::Lvl1MuCTPIInputPhase1* input, int bcid)
  {
    bool success = true;

    std::vector<LVL1MUONIF::Lvl1MuCTPIInputPhase1*> processedInputs;
    int nMSP = m_muonSectorProcessors.size();
    for (int i=0;i<nMSP;i++)
    {
      m_muonSectorProcessors[i]->setInput(input);
      m_muonSectorProcessors[i]->runOverlapRemoval(bcid);
      m_muonSectorProcessors[i]->makeL1TopoData(bcid);
      processedInputs.push_back(m_muonSectorProcessors[i]->getOutput());
    }

    //Run the trigger processor algorithms
    
    success = success && m_triggerProcessor->mergeInputs(processedInputs);
    success = success && m_triggerProcessor->computeMultiplicities(bcid);
    success = success && m_triggerProcessor->makeTopoSelections();
    return success;
  }

  LVL1::MuCTPIL1Topo SimController::getL1TopoData(int bcid)
  {
    LVL1::MuCTPIL1Topo l1topo;
    int nMSP = m_muonSectorProcessors.size();
    for (int i=0;i<nMSP;i++)
    {
      LVL1::MuCTPIL1Topo* l1topo_ptr = m_muonSectorProcessors[i]->getL1TopoData(bcid);
      if (l1topo_ptr) l1topo + *l1topo_ptr;
    }
    return l1topo;
  }

  TriggerProcessor* SimController::getTriggerProcessor()
  {
    return m_triggerProcessor;
  }

}

