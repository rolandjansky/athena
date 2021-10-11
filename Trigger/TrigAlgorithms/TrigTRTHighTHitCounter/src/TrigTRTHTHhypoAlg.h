/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGTRTHTHHYPOALG_H
#define TRIGTRTHTHHYPOALG_H 1

#include <string>

#include "GaudiKernel/StatusCode.h"
#include "DecisionHandling/HypoBase.h"
#include "xAODTrigRinger/TrigRNNOutput.h"
#include "xAODTrigRinger/TrigRNNOutputContainer.h"

#include "ITrigTRTHTHhypoTool.h"

/**
 * @class TrigTRTHTHhypoAlg
 * @author Jiri Masik <Jiri.Masik@hep.manchester.ac.uk>
 * @brief  TrigTRTHTHhypoAlg passes depending on the number of TRT hits found by the TrigTRTHTHCounter fex
 **/
class TrigTRTHTHhypoAlg : public ::HypoBase {
 public: 

  TrigTRTHTHhypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;

 private: 
  ToolHandleArray< ITrigTRTHTHhypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
     
  SG::ReadHandleKey< xAOD::TrigRNNOutputContainer > m_trigRNNOutputKey { this, "RNNOutputName", "RNNOutput", "output RNN container name " };
}; 

#endif
