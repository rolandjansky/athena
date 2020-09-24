/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
 * PFMuonFlowElementAssoc.h
 * Header file for class PFEMuonFlowElementAssoc
 *                                                                                                                                                             
 * Created by M.T. Anthony
 */
  

#ifndef PFMUONFLOWELEMENTASSOC_H
#define PFMUONFLOWELEMENTASSOC_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODPFlow/FlowElementContainer.h"
#include "StoreGate/WriteDecorHandle.h"

/**
   AthReentrantAlgorithm to associate Flow Elements (FEs) to Muons.
   Associates tracks from charged flow elements to Muons only (otherwise links to NULL)
**/

class PFMuonFlowElementAssoc : public AthReentrantAlgorithm {

public:
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  PFMuonFlowElementAssoc(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~PFMuonFlowElementAssoc();

  virtual StatusCode initialize();
  virtual StatusCode execute(const EventContext & ctx ) const;
  virtual StatusCode finalize();
  
private:
  
  // ReadHandleKeys
  SG::ReadHandleKey<xAOD::MuonContainer>m_muonReadHandleKey{this,"MuonContainer","Muons","ReadHandleKey for Muons"};
  
  SG::ReadHandleKey<xAOD::FlowElementContainer>m_neutralFEReadHandleKey{this,"JetEtMissNeutralFlowElementContainer","JetETMissNeutralFlowElements","ReadHandleKey for neutral FlowElements"};

  SG::ReadHandleKey<xAOD::FlowElementContainer>m_chargedFEReadHandleKey{this,"JetEtMissChargedFlowElementContainer","JetETMissChargedFlowElements","ReadHandleKey for charged FlowElements"};


  /* Write key for adding charged Flow Element link decorations to muons */
  SG::WriteDecorHandleKey<xAOD::MuonContainer> m_muonChargedFEWriteDecorKey;
  /* Write key for adding Muon link decorations to charged Flow Elements */
  SG::WriteDecorHandleKey<xAOD::FlowElementContainer> m_ChargedFEmuonWriteDecorKey;

  /* Write key for adding neutral Flow Element link decorations to muons */
  SG::WriteDecorHandleKey<xAOD::MuonContainer> m_muonNeutralFEWriteDecorKey;
  /* Write key for adding Muon link decorations to neutral Flow Elements */
  SG::WriteDecorHandleKey<xAOD::FlowElementContainer> m_NeutralFEmuonWriteDecorKey;

  Gaudi::Property<bool> m_LinkNeutralFEClusters{this,"m_LinkNeutralFEClusters",false,"Toggle usage of linkage of Neutral FlowElements - false by default"};


};

#endif // PFMUONFLOWELEMENTASSOC.H
