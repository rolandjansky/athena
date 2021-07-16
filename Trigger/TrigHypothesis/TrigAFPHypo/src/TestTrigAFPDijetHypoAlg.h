/*   
     Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration 
*/
#ifndef TRIGAFPHYPO_TESTTRIGAFPDIJETHYPOALG_H
#define TRIGAFPHYPO_TESTTRIGAFPDIJETHYPOALG_H

#include "DecisionHandling/HypoBase.h"

#include "Gaudi/Property.h" 
#include "xAODJet/JetContainer.h" 
#include "xAODJet/JetAuxContainer.h" 
#include "xAODJet/Jet.h" 
#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPTrack.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h" 
#include "StoreGate/ReadHandleKey.h" 
#include "StoreGate/WriteHandleKey.h"

#include "TestTrigAFPDijetHypoTool.h"

class TestTrigAFPDijetHypoAlg : public ::HypoBase
{
 public:
  TestTrigAFPDijetHypoAlg(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;   
  virtual StatusCode execute(const EventContext &context) const override;
 private:
  ToolHandleArray<TestTrigAFPDijetHypoTool> m_hypoTools{this, "HypoTools", {}};
  SG::ReadHandleKey< xAOD::JetContainer > m_inputJetsKey {this, "InputJetCollection", "HLT_xAOD__JetContainer_a4tcemsubjesISFS", "input jet container key"}; 
  SG::ReadHandleKey< xAOD::AFPTrackContainer > m_AFPtrackCollectionReadKey {this, "AFPTrackContainer", "HLT_AFPTrackContainer", "xAOD AFP track collection"};
  
};

#endif // TRIGAFPHYPO_TESTTRIGAFPDIJETHYPOALG_H
