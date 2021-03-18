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
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/FlowElementContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "PFO_FE_ComparisonPlots.h"
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
   
  
  /** ReadHandleKey to retrieve xAOD::PFOContainer */
  SG::ReadHandleKey<xAOD::PFOContainer> m_PFOContainerHandleKey{this,"PFOContainerName","JetETMissChargedParticleFlowObjects","ReadHandleKey for the PFO container"};

  /** ReadHandleKey to retrieve MuonContainer */
  SG::ReadHandleKey<xAOD::MuonContainer> m_MuonContainerHandleKey{this,"MuonContainerName","Muons","ReadHandleKey for the muon container"};

  /** ReadHandleKey to retrieve ElectronContainer */
  SG::ReadHandleKey<xAOD::ElectronContainer> m_ElectronContainerHandleKey{this,"ElectronContainerName","Electrons","ReadHandleKey for the electron container"};

  /** ReadHandleKey to retrieve PhotonContainer */
  SG::ReadHandleKey<xAOD::PhotonContainer> m_PhotonContainerHandleKey{this,"PhotonContainerName","Photons","ReadHandleKey for the photon container"};

  /** ReadHandleKey to retrieve TauJetContainer */
  SG::ReadHandleKey<xAOD::TauJetContainer> m_TauJetContainerHandleKey{this,"TauJetContainerName","TauJets","ReadHandleKey for the taujet container"};


  /** Pointer to class that deals with histograms for charged FE */
  std::unique_ptr<PFOChargedValidationPlots> m_FEChargedValidationPlots;

  /** Pointer to class that deals with histograms for neutral FE */
  std::unique_ptr<PFONeutralValidationPlots> m_FENeutralValidationPlots;

  /** Pointer to class that deals with histograms for lepton links */
  std::unique_ptr<LeptonCFEValidationPlots> m_LeptonLinkerPlots_CFE;
  std::unique_ptr<LeptonNFEValidationPlots> m_LeptonLinkerPlots_NFE;
  
  /** Select whether to use neutral or charged FE */
  bool m_useNeutralFE;

  /** Pointer to class that deals with PFO/FE comparison plots  */
  std::unique_ptr<PFO_FE_ComparisonPlots> m_charged_PFO_FE_comparison;
  std::unique_ptr<PFO_FE_ComparisonPlots> m_neutral_PFO_FE_comparison;

};
#endif
