/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  SG::ReadHandleKey<xAOD::VertexContainer> m_vertContainerKey{this,"VerticesKey","PrimaryVertices","RHK for primary veritces"};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackContainerKey{this,"TracksKey","InDetTrackParticles","RHK for ID tracks"};

  SG::ReadHandleKey<xAOD::JetContainer> m_jetContainerKey;
  SG::ReadHandleKey<xAOD::MuonContainer> m_muonContainerKey;
  SG::ReadHandleKey<xAOD::ElectronContainer> m_electronContainerKey;
  SG::ReadDecorHandleKey<xAOD::JetContainer> m_btagLinkKey{this,"BTagLinkKey","","RDHK for btag links"};
  SG::ReadDecorHandleKey<xAOD::BTaggingContainer> m_btagResultKey{this,"BTagResultKey","","RDHK for monitored BTag variables"};

  // isolation decoration keys
  SG::ReadDecorHandleKey<xAOD::MuonContainer> m_MuonEtIsoDecorKey { this, "MuonEtIsoDecorKey", "Muons.topoetcone20" };
  SG::ReadDecorHandleKey<xAOD::MuonContainer> m_MuonPtIsoDecorKey { this, "MuonPtIsoDecorKey", "Muons.ptvarcone30" };
  SG::ReadDecorHandleKey<xAOD::ElectronContainer> m_EleEtIsoDecorKey { this, "EleEtIsoDecorKey", "Electrons.topoetcone20" };
  SG::ReadDecorHandleKey<xAOD::ElectronContainer> m_ElePtIsoDecorKey { this, "ElePtIsoDecorKey", "Electrons.ptvarcone20" };

  bool m_skipJetFilter; //true for HI/HI-p collisions, false for pp collisions
  std::string m_trackParticleName;
  std::string m_primaryVertexName;
  std::string m_jetName;
  std::string m_electronName;
  std::string m_muonName;

  bool m_do_cuts;
  double m_trk_d0_min_cut;
  double m_trk_d0_max_cut;
  unsigned int m_pri_vtx_trk_min_cut;
  double m_jet_pt_cut;
  double m_jet_eta_cut;
  unsigned int m_trk_n;

  double m_JetPtCut;
  double m_JetEtaCut;
  double m_softMuonPtCut;
  double m_MuonPtCut;
  double m_MuonEtaCut;
  double m_ElectronPtCut;
  double m_ElectronEtaCut;
  double m_ElectronEtaCrackLowCut;
  double m_ElectronEtaCrackHighCut;
  double m_ElectronTopoEtCone20Cut;
  double m_ElectronPtVarCone20Cut;
  double m_MuonTopoEtCone20Cut;
  double m_MuonPtVarCone20Cut;
  double m_MuonPtVarCone30Cut;
 
  std::string m_mv_algorithmName;
  double m_mv_cFraction;
  double m_mv_bFraction;
  double m_mv_60_weight_cut;
  double m_mv_70_weight_cut;
  double m_mv_77_weight_cut;
  double m_mv_85_weight_cut;

  enum Jet_t { goodJet, suspectJet, badJet };
  void fillGoodJetHistos(const xAOD::Jet *jet) const;
  void fillLowLevelJetHistos(const xAOD::Jet *jet) const;
  void fillSuspectJetHistos(const xAOD::Jet *jet) const;
  void fillBadJetHistos(const xAOD::Jet *jet) const;
  void fillTTbarHistograms(const xAOD::Jet *jet) const;
  bool passJetFilterCuts(const xAOD::Jet *jet) const;
  bool passKinematicCuts(const xAOD::Jet *jet) const;
  bool passJVTCuts(const xAOD::Jet *jet) const;
  bool passMuonOverlap(const xAOD::Jet *jet) const;
  double getMVweight(const xAOD::Jet *jet) const;
  Jet_t getTaggabilityLabel(const xAOD::Jet *jet) const; 

  std::string m_ElectronTrigger_201X;
  std::string m_MuonTrigger_201X;

};
#endif
