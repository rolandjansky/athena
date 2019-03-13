/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
/********************************************************************
 *
 * NAME:     TrigTauRecDiscriminant.h
 * PACKAGE:  Trigger/TrigAlgorithms/TrigTauDiscriminant
 *
 * AUTHOR:   M. Morgenstern (based on tauDiscriBuilder)
 * CREATED:  
  *********************************************************************/

#ifndef TRIGTAUDISCRIBUILDER_H
#define TRIGTAUDISCRIBUILDER_H

#include "TrigInterfaces/FexAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
//#include "TauDiscriminant/TauDiscriToolBase.h"
#include "tauRecTools/ITauToolBase.h"
#include <vector>

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;  // legacy trigger code

namespace HLT {
  class TriggerElement;
}

class TrigTauDiscriBuilder: public HLT::FexAlgo {
 public:
  /**  constructor */
  TrigTauDiscriBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  /** destructor */
  ~TrigTauDiscriBuilder();
  // Gaudi algorithm hooks

  /** HLT method to initialize */
  HLT::ErrorCode  hltInitialize();

  /** HLT method to finalize */
  HLT::ErrorCode hltFinalize();

  /** HLT method to execute FEX algo on a given TE.
   input is last TE from EF ID, output is TE for EF tauRec hypo execution */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);

 private:
  /** internal tool store */
  ToolHandleArray<ITauToolBase>  m_tools;

  /** vector of Timers */
  std::vector<TrigTimer* > m_mytimers;

  /** Monitoring : m_LLHScore obtained */
  double  m_LLHScore; 

  /** Monitoring : m_BDTScore obtained */
  double  m_BDTScore; 

};
#endif
