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


  /** Write key for adding charged Flow Element link decorations to muons **/
  SG::WriteDecorHandleKey<xAOD::MuonContainer> m_muonChargedFEWriteHandleKey{this,"MuonContainer","Muons.chargedFELinks","WriteHandleKey for muon link to charged FlowElements"};

  /** Write key for adding Muon link decorations to charged Flow Elements **/
  SG::WriteDecorHandleKey<xAOD::FlowElementContainer> m_ChargedFEmuonWriteHandleKey{this,"FlowElementOutputName","JetETMissChargedFlowElements.FE_MuonLinks","WriteHandleKey for Charged Flow Elements coupled to muons"};

  /** Write key for adding neutral Flow Element link decorations to muons **/
  SG::WriteDecorHandleKey<xAOD::MuonContainer> m_muonNeutralFEWriteHandleKey{this,"MuonContainer","Muons.neutralFELinks","WriteHandleKey for muon links to neutral FlowElement"};

  /** Write key for adding Muon link decorations to neutral Flow Elements **/
  SG::WriteDecorHandleKey<xAOD::FlowElementContainer> m_NeutralFEmuonWriteHandleKey{this,"FlowElementContainer","JetETMissNeutralFlowElements.MuonLinks","WriteHandleKey for neutral flow Elements to Muons"};

  /** Write key for adding fraction of energy used in cell matching decorations to muons **/
  SG::WriteDecorHandleKey<xAOD::FlowElementContainer> m_NeutralFE_efrac_match_muonWriteHandleKey{this,"FlowElementContainer","JetETMissNeutralFlowElements.FE_efrac_matched_muon","WriteHandleKey for neutral FlowElements to Muons"};

  /***Gaudi Property to configure linkage of Neutral Flow Elements to Muon clusters (EXPERIMENTAL - default = False/OFF) **/
  Gaudi::Property<bool> m_LinkNeutralFEClusters{this,"m_LinkNeutralFEClusters",false,"Toggle usage of linkage of Neutral FlowElements - false by default (EXPERIMENTAL)"};

  /** 
      (EXPERIMENTAL) Gaudi Property to configure linkage of Neutral FEs to TopoClusters associated to Muons. Only works with previous option set to True (m_LinkNeutralFEClusters). 
      True: Link FEs to Topoclusters associated to Muons
      False: Link FEs to CaloClusters associated to Muons
 **/
  Gaudi::Property<bool> m_UseMuonTopoClusters{this,"m_UseMuonTopoClusters",false,"Toggle usage of linker of muon associated topoclusters to flow elements - false by default (EXPERIMENTAL)"};
};
#endif // PFMUONFLOWELEMENTASSOC.H
