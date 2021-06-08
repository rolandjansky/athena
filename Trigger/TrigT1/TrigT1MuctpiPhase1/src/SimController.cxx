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

  std::string SimController::processData(LVL1MUONIF::Lvl1MuCTPIInputPhase1* input, int bcid)
  {
    std::string ret = "";
    std::vector<const LVL1MUONIF::Lvl1MuCTPIInputPhase1*> processedInputs;
    for (MuonSectorProcessor& msp : m_muonSectorProcessors )
    {
      msp.setInput(input);
      msp.runOverlapRemoval(bcid);
      if ((ret = msp.makeL1TopoData(bcid)) != "") return ret;
      processedInputs.push_back(msp.getOutput());
    }

    //Run the trigger processor algorithms
    
    m_triggerProcessor.mergeInputs(processedInputs);
    if ((ret = m_triggerProcessor.computeMultiplicities(bcid)) != "") return ret;
    m_triggerProcessor.makeTopoSelections();
    return "";
  }

  LVL1::MuCTPIL1Topo SimController::getL1TopoData(int bcid) const
  {
    LVL1::MuCTPIL1Topo l1topo;
    for (const MuonSectorProcessor& msp : m_muonSectorProcessors )
    {
      const LVL1::MuCTPIL1Topo* l1topo_ptr = msp.getL1TopoData(bcid);
      if (l1topo_ptr) l1topo += *l1topo_ptr;
    }
    return l1topo;
  }

}
