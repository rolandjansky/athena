/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGTRTHTHHYPO_MT_H
#define TRIGTRTHTHHYPO_MT_H 1

#include <string>

#include "GaudiKernel/StatusCode.h"
#include "DecisionHandling/HypoBase.h"
#include "xAODTrigRinger/TrigRNNOutput.h"
#include "xAODTrigRinger/TrigRNNOutputContainer.h"

#include "ITrigTRTHTHhypoTool.h"

/**
 * @class TrigTRTHTHhypoMT
 * @author Jiri Masik <Jiri.Masik@hep.manchester.ac.uk>
 * @brief  TrigTRTHTHhypo passes depending on the number of TRT hits found by the TrigTRTHTHCounterMT fex
 **/
class TrigTRTHTHhypoMT : public ::HypoBase {
 public: 

  TrigTRTHTHhypoMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;

 private: 
  ToolHandleArray< ITrigTRTHTHhypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
     
  SG::ReadHandleKey< xAOD::TrigRNNOutputContainer > m_trigRNNOutputKey { this, "RNNOutputName", "RNNOutput", "output RNN container name " };
}; 

#endif //> !TRIGTRTHTHHYPO_MT_H

