/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LARNOISYROALG_H
#define LARCELLREC_LARNOISYROALG_H

/** 
@class LArNoisyROAlg
@brief Find list of suspicious preamplifiers and Front End Boards from cell collection

 Created September 28, 2009  L. Duflot
 Modified May, 2014 B.Trocme 
 - Remove saturated medium cut
 - Create a new weighted Std algorithm

*/



#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloInterface/ILArNoisyROTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "LArRecEvent/LArNoisyROSummary.h"
#include "xAODEventInfo/EventInfo.h"

class CaloCellContainer;

class LArNoisyROAlg : public AthReentrantAlgorithm
{
 public:

  LArNoisyROAlg(const std::string &name,ISvcLocator *pSvcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute (const EventContext& ctx) const override;   
  virtual StatusCode finalize() override;

 
 private: 
  ToolHandle<ILArNoisyROTool> m_noisyROTool;

  Gaudi::Property<bool>  m_isMC     { this, "isMC", false, "Are we working with simu?" };
 
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "eventInfoKey", "EventInfo", "Key for EventInfo object"};
  SG::ReadHandleKey<CaloCellContainer> m_CaloCellContainerName {this, "CaloCellContainer", "AllCalo", "input cell container key"};
  SG::WriteHandleKey<LArNoisyROSummary> m_outputKey {this, "OutputKey", "LArNoisyROSummary", "output object key"};
  SG::ReadCondHandleKey<LArBadFebCont> m_knownBadFEBsVecKey {this, "BadFEBsKey", "LArKnownBadFEBs", "key to read the known Bad FEBs"};
  SG::ReadCondHandleKey<LArBadFebCont> m_knownMNBFEBsVecKey {this, "MNBFEBsKey", "LArKnownMNBFEBs", "key to read the known MNB FEBs"};

};


#endif
