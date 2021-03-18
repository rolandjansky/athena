/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TrigBmumuxComboHypo_H
#define TRIG_TrigBmumuxComboHypo_H

#include <string>
#include <vector>
#include <utility>

#include "Gaudi/Property.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "DecisionHandling/ComboHypo.h"

#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include "ITrackToVertex/ITrackToVertex.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "ITrigBphysState.h"
#include "TrigBmumuxComboHypoTool.h"

#include "Constants.h"
typedef struct PDG20 PDG;


/**
 * @class TrigBmumuxState
 * @brief State class for TrigBmumuxComboHypo algorithm
 */
class TrigBmumuxState: public ::ITrigBphysState {
 public:
  const EventContext* context;
  const TrigCompositeUtils::DecisionContainer* previousDecisions;
  TrigCompositeUtils::DecisionContainer* decisions;
  xAOD::TrigBphysContainer* trigBphysCollection;
  struct Muon {
    ElementLink<xAOD::MuonContainer> link;
    ElementLinkVector<TrigCompositeUtils::DecisionContainer> decisionLinks;
    TrigCompositeUtils::DecisionIDContainer decisionIDs;
  };
  std::vector<Muon> muons;
  std::vector<ElementLink<xAOD::TrackParticleContainer>> tracks;
  xAOD::VertexContainer dimuons;
  xAOD::VertexAuxContainer dimuonsStore;

  StatusCode addTriggerObject(xAOD::TrigBphys* triggerObject) {
    if (!triggerObject) {
      return StatusCode::FAILURE;
    }
    trigBphysCollection->push_back(triggerObject);
    return StatusCode::SUCCESS;
  }
};


/**
 * @class TrigBmumuxComboHypo
 * @brief EF hypothesis algorithm for B -> mu+ mu- X decays:
 *        B+ -> mu+ mu- K+
 *        B_s0 -> mu+ mu- phi1020(-> K+, K-)
 */
class TrigBmumuxComboHypo: public ::ComboHypo {
 public:
  TrigBmumuxComboHypo(const std::string& name, ISvcLocator* pSvcLocator);
  TrigBmumuxComboHypo() = delete;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;

  enum Decay : size_t {
    kPsi_2mu,      // psi -> mu+ mu-
    kB_2mu1trk,    // B -> mu+ mu- trk1
    kB_2mu2trk     // B -> mu+ mu- trk1 trk2
  };

 private:
  std::unique_ptr<TrigBmumuxState> makeState(const EventContext* context,
                                             const TrigCompositeUtils::DecisionContainer* previousDecisions,
                                             TrigCompositeUtils::DecisionContainer* decisions,
                                             xAOD::TrigBphysContainer* trigBphysCollection) const;
  StatusCode mergeMuonsFromViews(TrigBmumuxState&) const;
  StatusCode mergeTracksFromViews(TrigBmumuxState&) const;
  StatusCode findDimuonCandidates(TrigBmumuxState&) const;
  StatusCode findBmumuxCandidates(TrigBmumuxState&) const;
  StatusCode createDecisionObjects(TrigBmumuxState&) const;
  xAOD::Vertex* fit(const EventContext* context, const std::vector<ElementLink<xAOD::TrackParticleContainer>>& tracklist,
                                     Decay = kPsi_2mu, const xAOD::TrigBphys* dimuon = nullptr) const;
  xAOD::TrigBphys* makeTriggerObject(const xAOD::Vertex*,
                                     xAOD::TrigBphys::pType type = xAOD::TrigBphys::MULTIMU,
                                     const std::vector<double>& trkMass = {PDG::mMuon, PDG::mMuon},
                                     const ElementLink<xAOD::TrigBphysContainer>& dimuonLink = ElementLink<xAOD::TrigBphysContainer>()) const;

  bool isIdenticalTracks(const xAOD::TrackParticle* lhs, const xAOD::TrackParticle* rhs) const;
  bool isIdenticalTracks(const xAOD::Muon* lhs, const xAOD::Muon* rhs) const;
  bool passDimuonTrigger(const std::vector<const TrigCompositeUtils::DecisionIDContainer*>& previousDecisionIDs) const;
  bool isInMassRange(double mass, const std::pair<double, double>& range) const { return (mass > range.first && mass < range.second); }

  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackParticleContainerKey {this,
    "TrackCollectionKey", "InDetTrackParticles", "input TrackParticle container name"};
  SG::ReadHandleKey<xAOD::MuonContainer> m_muonContainerKey {this,
    "MuonCollectionKey", "Muons", "input EF Muon container name"};
  SG::WriteHandleKey<xAOD::TrigBphysContainer> m_trigBphysContainerKey {this,
    "TrigBphysCollectionKey", "TrigBphysContainer", "output TrigBphysContainer name"};

  // general properties
  Gaudi::Property<double> m_deltaR {this,
    "DeltaR", 0.01, "minimum deltaR between same-sign tracks (overlap removal)"};
  Gaudi::Property<double> m_trkZ0 {this,
    "TrkZ0", 50., "maximum z0 impact parameter of the track wrt the fitted dimuon vertex; no preselection if negative"};
  Gaudi::Property<unsigned int> m_maxFitAttempts_DimuTrk1 {this,
    "MaxFitAttempts_DimuTrk1", 200, "maximum vertex fitter calls for dimu+trk1 decays (time-out protect)"};
  Gaudi::Property<unsigned int> m_maxFitAttempts_DimuTrk1Trk2 {this,
    "MaxFitAttempts_DimuTrk1Trk2", 2000, "maximum vertex fitter calls for dimu+trk1+trk2 decays (time-out protect)"};

  // dimuon properties
  Gaudi::Property<bool> m_dimuon_rejectSameChargeTracks {this,
    "Dimuon_rejectSameChargeTracks", true, "if true, the only (mu+, mu-) pairs will be kept (no wrong-charge combinations)"};
  Gaudi::Property<std::pair<double, double>> m_dimuon_massRange {this,
    "Dimuon_massRange", {100., 5500.}, "dimuon mass range"};
  Gaudi::Property<double> m_dimuon_chi2 {this,
    "Dimuon_chi2", 20., "maximum chi2 of the dimuon vertex"};

  // B+ -> mu+ mu- K+
  Gaudi::Property<bool> m_BplusToMuMuKaon {this,
    "BplusToMuMuKaon", true, "switch on/off B+ -> mu+ mu- K+ decay"};
  Gaudi::Property<double> m_BplusToMuMuKaon_minKaonPt {this,
    "BplusToMuMuKaon_minKaonPt", 100., "minimum pT of kaon track"};
  Gaudi::Property<std::pair<double, double>> m_BplusToMuMuKaon_massRange {this,
    "BplusToMuMuKaon_massRange", {4500., 5900.}, "B+ mass range"};
  Gaudi::Property<float> m_BplusToMuMuKaon_chi2 {this,
    "BplusToMuMuKaon_chi2", 50., "maximum chi2 of the fitted B+ vertex"};

  // B_c+ -> J/psi(-> mu+ mu-) pi+
  Gaudi::Property<bool> m_BcToMuMuPion {this,
    "BcToMuMuPion", true, "switch on/off B_c+ -> J/psi(-> mu+ mu-) pi+ decay"};
  Gaudi::Property<double> m_BcToMuMuPion_minPionPt {this,
    "BcToMuMuPion_minPionPt", 2000., "minimum pT of pion track"};
  Gaudi::Property<std::pair<double, double>> m_BcToMuMuPion_dimuonMassRange {this,
    "BcToMuMuPion_dimuonMassRange", {2500., 4300.}, "dimuon mass range for B_c+ decay"};
  Gaudi::Property<std::pair<double, double>> m_BcToMuMuPion_massRange {this,
    "BcToMuMuPion_massRange", {5500., 7300.}, "B_c+ mass range"};
  Gaudi::Property<float> m_BcToMuMuPion_chi2 {this,
    "BcToMuMuPion_chi2", 50., "maximum chi2 of the fitted B_c+ vertex"};

  // B_s0 -> mu+ mu- phi(-> K+ K-)
  Gaudi::Property<bool> m_BsToMuMuPhi1020 {this,
    "BsToMuMuPhi1020", true, "switch on/off B_s0 -> mu+ mu- phi(-> K+ K-) decay"};
  Gaudi::Property<bool> m_BsToMuMuPhi1020_rejectSameChargeTracks {this,
    "BsToMuMuPhi1020_rejectSameChargeTracks", true, "if true, the only (K+, K-) pairs will be kept (no wrong-charge combinations)"};
  Gaudi::Property<double> m_BsToMuMuPhi1020_minKaonPt {this,
    "BsToMuMuPhi1020_minKaonPt", 100., "minimum pT of kaon tracks"};
  Gaudi::Property<std::pair<double, double>> m_BsToMuMuPhi1020_massRange {this,
    "BsToMuMuPhi1020_massRange", {4800., 5800.}, "B_s0 mass range"};
  Gaudi::Property<std::pair<double, double>> m_BsToMuMuPhi1020_phiMassRange {this,
    "BsToMuMuPhi1020_phiMassRange", {940., 1100.}, "phi1020 mass range"};
  Gaudi::Property<float> m_BsToMuMuPhi1020_chi2 {this,
    "BsToMuMuPhi1020_chi2", 60., "maximum chi2 of the fitted B+ vertex"};

  // B0 -> mu+ mu- K*0(-> K+ pi-)
  Gaudi::Property<bool> m_BdToMuMuKstar0 {this,
    "BdToMuMuKstar0", true, "switch on/off B0 -> mu+ mu- K*0(-> K+ pi-) decay"};
  Gaudi::Property<bool> m_BdToMuMuKstar0_rejectSameChargeTracks {this,
    "BdToMuMuKstar0_rejectSameChargeTracks", true, "if true, the only (K+, pi-) and (K-, pi+) pairs will be kept (no wrong-charge combinations)"};
  Gaudi::Property<double> m_BdToMuMuKstar0_minKaonPt {this,
    "BdToMuMuKstar0_minKaonPt", 100., "minimum pT of kaon track"};
  Gaudi::Property<double> m_BdToMuMuKstar0_minPionPt {this,
    "BdToMuMuKstar0_minPionPt", 100., "minimum pT of pion track"};
  Gaudi::Property<std::pair<double, double>> m_BdToMuMuKstar0_massRange {this,
    "BdToMuMuKstar0_massRange", {4600., 5900.}, "B0 mass range"};
  Gaudi::Property<std::pair<double, double>> m_BdToMuMuKstar0_KstarMassRange {this,
    "BdToMuMuKstar0_KstarMassRange", {700., 1100.}, "K*0 mass range"};
  Gaudi::Property<float> m_BdToMuMuKstar0_chi2 {this,
    "BdToMuMuKstar0_chi2", 60., "maximum chi2 of the fitted B0 vertex"};

  // Lambda_b0 -> J/psi(-> mu+ mu-) p K-
  Gaudi::Property<bool> m_LambdaBToMuMuProtonKaon {this,
    "LambdaBToMuMuProtonKaon", true, "switch on/off Lambda_b0 -> J/psi(-> mu+ mu-) p K- decay"};
  Gaudi::Property<bool> m_LambdaBToMuMuProtonKaon_rejectSameChargeTracks {this,
    "LambdaBToMuMuProtonKaon_rejectSameChargeTracks", false, "if true, the only (p, K-) and (anti-p, K+) pairs will be kept (no wrong-charge combinations)"};
  Gaudi::Property<double> m_LambdaBToMuMuProtonKaon_minProtonPt {this,
    "LambdaBToMuMuProtonKaon_minProtonPt", 1000., "minimum pT of proton track"};
  Gaudi::Property<double> m_LambdaBToMuMuProtonKaon_minKaonPt {this,
    "LambdaBToMuMuProtonKaon_minKaonPt", 1000., "minimum pT of kaon track"};
  Gaudi::Property<double> m_LambdaBToMuMuProtonKaon_minKstarMass {this,
    "LambdaBToMuMuProtonKaon_minKstarMass", 1300., "min value for both mass(trk1=kaon, trk2=pion) and mass(trk1=pion, trk2=kaon)"};
  Gaudi::Property<std::pair<double, double>> m_LambdaBToMuMuProtonKaon_dimuonMassRange {this,
    "LambdaBToMuMuProtonKaon_dimuonMassRange", {2500., 4300.}, "dimuon mass range for Lambda_b0 decay"};
  Gaudi::Property<std::pair<double, double>> m_LambdaBToMuMuProtonKaon_massRange {this,
    "LambdaBToMuMuProtonKaon_massRange", {4800., 6400.}, "Lambda_b0 mass range"};
  Gaudi::Property<float> m_LambdaBToMuMuProtonKaon_chi2 {this,
    "LambdaBToMuMuProtonKaon_chi2", 60., "maximum chi2 of the fitted Lambda_b0 vertex"};

  // external tools
  ToolHandle<InDet::VertexPointEstimator> m_vertexPointEstimator {this,
    "VertexPointEstimator", "", "tool to find starting point for the vertex fitter"};
  ToolHandle<Trk::TrkVKalVrtFitter> m_vertexFitter {this,
    "VertexFitter", "", "VKalVrtFitter tool to fit tracks into the common vertex"};
  ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool {this,
    "TrackToVertexTool", "", "tool to extrapolate track to vertex or beamspot"};
  ToolHandle<GenericMonitoringTool> m_monTool {this,
    "MonTool", "", "monitoring tool"};

  TrigCompositeUtils::DecisionIDContainer m_allowedIDs;

  const std::vector<std::vector<double>> m_trkMass{
    {PDG::mMuon, PDG::mMuon},
    {PDG::mMuon, PDG::mMuon, PDG::mKaon},
    {PDG::mMuon, PDG::mMuon, PDG::mKaon, PDG::mKaon}
  };

};

#endif  // TRIG_TrigBmumuxComboHypo_H

