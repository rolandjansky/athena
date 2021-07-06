/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGMONITORALGORITHM_H
#define JETTAGMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadHandle.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "VxVertex/VxContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include <string>
#include <vector>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include <bitset>
#include <map>

class JetTagMonitorAlgorithm : public AthMonitorAlgorithm {
 public:
  JetTagMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~JetTagMonitorAlgorithm();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

 private:
  SG::ReadHandleKey<xAOD::VertexContainer> m_VertContainerKey{this,"VerticesKey","PrimaryVertices","RHK for primary vertices"};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_TrackContainerKey{this,"TracksKey","InDetTrackParticles","RHK for ID tracks"};

  SG::ReadHandleKey<xAOD::JetContainer> m_JetContainerKey;
  SG::ReadHandleKey<xAOD::MuonContainer> m_MuonContainerKey;
  SG::ReadHandleKey<xAOD::ElectronContainer> m_ElectronContainerKey;
  SG::ReadDecorHandleKey<xAOD::JetContainer> m_btagLinkKey{this,"BTagLinkKey","","RDHK for btag links"};
  SG::ReadDecorHandleKey<xAOD::BTaggingContainer> m_btagResultKey{this,"BTagResultKey","","RDHK for monitored BTag variables"};

  SG::ReadDecorHandleKey<xAOD::MuonContainer> m_MuonEtIsoDecorKey {this,"MuonEtIsoDecorKey","Muons.topoetcone20"};
  SG::ReadDecorHandleKey<xAOD::MuonContainer> m_MuonPtIsoDecorKey {this,"MuonPtIsoDecorKey","Muons.ptvarcone30"};
  SG::ReadDecorHandleKey<xAOD::ElectronContainer> m_EleEtIsoDecorKey {this,"EleEtIsoDecorKey","Electrons.topoetcone20"};
  SG::ReadDecorHandleKey<xAOD::ElectronContainer> m_ElePtIsoDecorKey {this,"ElePtIsoDecorKey","Electrons.ptvarcone20"};

  bool m_SkipJetFilter; //true for HI/HI-p collisions, false for pp collisions
  bool m_DoExtraTaggerHistos; //true if interested in IP2D, IP3D, SV1, JetFitter, RRNIP and MV2c10 (r21) histograms

  int m_MinGoodTrackCut;
  float m_TrackPtCut;
  float m_TrackEtaCut;
  float m_Trackd0Cut;
  float m_Trackz0sinCut;
  float m_TrackChi2ndfCut;
  int m_TrackHitIBLCut;
  int m_TrackHitPixCut;
  int m_TrackHolePixCut;
  int m_TrackHitSCTCut;
  int m_TrackHoleSCTCut;
  int m_TrackHitSiCut;
  int m_TrackHoleSiCut;

  float m_JetPtCut;
  float m_JetEtaCut;
  float m_SoftMuonPtCut;
  float m_MuonPtCut;
  float m_MuonEtaCut;
  float m_ElectronPtCut;
  float m_ElectronEtaCut;
  float m_ElectronEtaCrackLowCut;
  float m_ElectronEtaCrackHighCut;
  float m_ElectronTopoEtCone20Cut;
  float m_ElectronPtVarCone20Cut;
  float m_MuonTopoEtCone20Cut;
  float m_MuonPtVarCone30Cut;
  float m_JVTCut;
  float m_JVTpTCut;
  float m_JVTetaCut;
 
  std::string m_TaggerName;
  float m_cFraction;
  float m_bFraction;
  float m_WP60Cut;
  float m_WP70Cut;
  float m_WP77Cut;
  float m_WP85Cut;

  enum Jet_t {goodJet, suspectJet, badJet};
  void fillGoodJetHistos(const xAOD::Jet *jet) const;
  void fillSuspectJetHistos(const xAOD::Jet *jet) const;
  void fillJetTracksHistos(const xAOD::Jet *jet, float PV_Z)  const;
  void fillExtraTaggerHistos(const xAOD::Jet *jet) const;
  void fillTTbarEventJetHistos(const xAOD::Jet *jet) const;
  bool passJetFilterCut(const xAOD::Jet *jet) const;
  bool passKinematicCut(const xAOD::Jet *jet) const;
  bool passJVTCut(const xAOD::Jet *jet) const;
  double getTaggerWeight(const xAOD::Jet *jet) const;
  Jet_t getQualityLabel(const xAOD::Jet *jet, float PV_Z) const; 

  std::string m_ElectronTrigger_201X;
  std::string m_MuonTrigger_201X;

};
#endif
