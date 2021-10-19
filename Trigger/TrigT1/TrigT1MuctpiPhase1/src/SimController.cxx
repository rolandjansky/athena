/*                                                                                                                      
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// First the corresponding header.
#include "SimController.h"

// The headers from other ATLAS packages,
// from most to least dependent.
#include "TrigT1Interfaces/Lvl1MuCTPIInputPhase1.h"
#include "TrigT1Interfaces/MuCTPIL1Topo.h"

// Headers from external packages.


// System headers.
#include <iostream>
#include <vector>

namespace LVL1MUCTPIPHASE1 {

  std::vector<std::string> SimController::configureTopo(const std::string& barrelFileName,
							const std::string& ecfFileName,
							const std::string& side0LUTFileName,
							const std::string& side1LUTFileName)
  {
    std::vector<std::string> errors;
    bool success = m_l1topoLUT.initializeLUT(barrelFileName, ecfFileName, side0LUTFileName, side1LUTFileName);
    if (!success) errors = m_l1topoLUT.getErrors();
    for (MuonSectorProcessor& msp : m_muonSectorProcessors )
    {
      msp.setL1TopoLUT(&m_l1topoLUT);
    }
    return errors;
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

  std::string SimController::processData(LVL1MUONIF::Lvl1MuCTPIInputPhase1* input, MUCTPIResults& results, int bcid) const
  {
    std::string ret = "";
    std::vector<const LVL1MUONIF::Lvl1MuCTPIInputPhase1*> processedInputs;
    for (const MuonSectorProcessor& msp : m_muonSectorProcessors )
    {
      msp.runOverlapRemoval(input, bcid);
      if ((ret = msp.makeL1TopoData(input, bcid, results.l1topoData)) != "") return ret;
      processedInputs.push_back(input);
    }

    //Run the trigger processor algorithms    
    LVL1MUONIF::Lvl1MuCTPIInputPhase1 mergedInputs = m_triggerProcessor.mergeInputs(processedInputs);
    if ((ret = m_triggerProcessor.computeMultiplicities(mergedInputs, bcid, results)) != "") return ret;
    return "";
  }
}
