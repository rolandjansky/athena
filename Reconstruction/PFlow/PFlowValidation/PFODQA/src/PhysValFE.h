/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHYSVALFE_H
#define PHYSVALFE_H

#include "PFOChargedValidationPlots.h"
#include "PFONeutralValidationPlots.h"
#include "LeptonNFEValidationPlots.h"
#include "LeptonCFEValidationPlots.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include <string>
#include "xAODTracking/VertexContainer.h"
#include "xAODPFlow/FlowElementContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadDecorHandle.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODEventInfo/EventInfo.h"

class PhysValFE : public ManagedMonitorToolBase {

public:

  /** Standard Constructor */
  PhysValFE (const std::string& type, const std::string& name, const IInterface* parent );

  /** Standard Destructor */
  virtual ~PhysValFE();
  
  /** Standard AlgTool Functions */
  virtual StatusCode initialize();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

 private:
  
  /** ReadHandle to retrieve xAOD::VertexContainer */
  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainerReadHandleKey{this,"primaryVerticesName","PrimaryVertices","ReadHandleKey for the PrimaryVertices container"};

  /** ReadHandle to retrieve xAOD::FlowElementContainer (charged) */
  SG::ReadHandleKey<xAOD::FlowElementContainer> m_FEContainerHandleKey{this,"FlowElementContainerName","JetETMissChargedFlowElements","ReadHandleKey for the FE container"};  

  /** Read key for getting charged Flow Element link decorations to muons **/
  SG::ReadDecorHandleKey<xAOD::MuonContainer> m_muonChargedFEReadHandleKey{this,"MuonContainer_chargedFELinks","Muons.chargedFELinks","ReadHandleKey for muon link to charged FlowElements"};

  /** Read key for getting neutral Flow Element link decorations to muons **/
  SG::ReadDecorHandleKey<xAOD::MuonContainer> m_muonNeutralFEReadHandleKey{this,"MuonContainer_neutralFELinks","Muons.neutralFELinks","ReadHandleKey for muon links to neutral FlowElement"}; 

  /** Read key for getting charged Flow Element link decorations to electrons **/
  SG::ReadDecorHandleKey<xAOD::ElectronContainer> m_electronChargedFEReadHandleKey{this,"ElectronContainer_chargedFELinks","Electrons.chargedFELinks","ReadHandleKey for electron link to charged FlowElements"};

  /** Read key for getting neutral Flow Element link decorations to electrons **/
  SG::ReadDecorHandleKey<xAOD::ElectronContainer> m_electronNeutralFEReadHandleKey{this,"ElectronContainer_neutralFELinks","Electrons.neutralFELinks","ReadHandleKey for electron links to neutral FlowElement"}; 

  /** Read key for getting charged Flow Element link decorations to photons **/
  SG::ReadDecorHandleKey<xAOD::PhotonContainer> m_photonChargedFEReadHandleKey{this,"PhotonContainer_chargedFELinks","Photons.chargedFELinks","ReadHandleKey for photon link to charged FlowElements"};

  /** Read key for getting neutral Flow Element link decorations to photons **/
  SG::ReadDecorHandleKey<xAOD::PhotonContainer> m_photonNeutralFEReadHandleKey{this,"PhotonContainer_neutralFELinks","Photons.neutralFELinks","ReadHandleKey for photon links to neutral FlowElement"};   

  /** Read key for getting charged Flow Element link decorations to taus **/
  SG::ReadDecorHandleKey<xAOD::TauJetContainer> m_tauJetChargedFEReadHandleKey{this,"TauJetContainer_chargedFELinks","TauJets.chargedFELinks","ReadHandleKey for tau link to charged FlowElements"};

  /** Read key for getting neutral Flow Element link decorations to taus **/
  SG::ReadDecorHandleKey<xAOD::TauJetContainer> m_tauJetNeutralFEReadHandleKey{this,"TauJetContainer_neutralFELinks","TauJets.neutralFELinks","ReadHandleKey for tau links to neutral FlowElement"};    

  /** ReadHandleKey for EventInfo */
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoReadHandleKey{this,"EventInfoName","EventInfo","ReadHandleKey for EventInfo"};

  /** Pointer to class that deals with histograms for charged FE */
  std::unique_ptr<PFOChargedValidationPlots> m_FEChargedValidationPlots;

  /** Pointer to class that deals with histograms for neutral FE */
  std::unique_ptr<PFONeutralValidationPlots> m_FENeutralValidationPlots;

  /** Pointer to class that deals with histograms for lepton links */
  std::unique_ptr<LeptonCFEValidationPlots> m_LeptonLinkerPlots_CFE;
  std::unique_ptr<LeptonNFEValidationPlots> m_LeptonLinkerPlots_NFE;
  
  /** Select whether to use neutral or charged FE */
  bool m_useNeutralFE;
  

};
#endif
