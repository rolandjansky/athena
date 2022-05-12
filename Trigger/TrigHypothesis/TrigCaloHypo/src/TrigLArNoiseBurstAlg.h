/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGCALOHYPO_TRIGLARNOISEBURSTALG
#define TRIGCALOHYPO_TRIGLARNOISEBURSTALG

#include <string>
#include <mutex>

#include "GaudiKernel/ToolHandle.h"
#include "DecisionHandling/HypoBase.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "TrigCaloHypo/ITrigLArNoiseBurstHypoTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "LArRecConditions/LArBadChannelCont.h"

/**
 * @class TrigLArNoiseBurstAlg
 * @brief Implements LArNoiseBurst detection for the new HLT framework
 **/
class TrigLArNoiseBurstAlg : public ::HypoBase {
 public: 

  TrigLArNoiseBurstAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;

 private: 
  SG::ReadHandleKey<bool> m_noiseBurstDetected{this, "NoiseBurstKey", "HLT_NoiseBurst", "Flag which records if a noise burst was detected in the event"};
  ToolHandleArray< ITrigLArNoiseBurstHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };

  std::string m_name_of_is;
}; 
#endif
