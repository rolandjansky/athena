/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGCALOHYPO_TRIGLARNOISEBURSTALG
#define TRIGCALOHYPO_TRIGLARNOISEBURSTALG

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthViews/View.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "DecisionHandling/HypoBase.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "StoreGate/ReadHandleKey.h"



#include "TrigCaloHypo/ITrigLArNoiseBurstHypoTool.h"

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
  SG::ReadHandleKey<CaloCellContainer > m_cellContainerKey { this, "CellContainerKey", "CellsClusters","SG Key of cells"};
  SG::ReadCondHandleKey<LArBadFebCont> m_knownBadFEBsVecKey {this, "BadFEBsKey", "LArKnownBadFEBs", "key to read the known Bad FEBs"};
  SG::ReadCondHandleKey<LArBadFebCont> m_knownMNBFEBsVecKey {this, "MNBFEBsKey", "LArKnownMNBFEBs", "key to read the known MNB FEBs"};
  ToolHandleArray< ITrigLArNoiseBurstHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
}; 
#endif
