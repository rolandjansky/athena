/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TrigBmuxComboHypo_H
#define TRIG_TrigBmuxComboHypo_H

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
 * @class TrigBmuxState
 * @brief State class for TrigBmuxComboHypo algorithm
 */
class TrigBmuxState: public ::ITrigBphysState {
 public:
  TrigBmuxState() = delete;
  TrigBmuxState(const EventContext& context,
                const TrigCompositeUtils::DecisionContainer& previousDecisions,
                TrigCompositeUtils::DecisionContainer& decisions,
                xAOD::TrigBphysContainer* trigBphysCollection = nullptr,
                const InDet::BeamSpotData* beamSpotData = nullptr)
      : ITrigBphysState(context, previousDecisions, decisions, trigBphysCollection, beamSpotData) {}
  virtual ~TrigBmuxState() = default;

  struct Muon {
    ElementLink<xAOD::MuonContainer> link;
    ElementLink<TrigCompositeUtils::DecisionContainer> decisionLink;
    TrigCompositeUtils::DecisionIDContainer decisionIDs;
  };
  std::vector<Muon> muons;
  std::vector<size_t> trigBphysMuonIndices;
  StatusCode addTriggerObject(xAOD::TrigBphys* triggerObject, size_t muonIndex) {
    if (!triggerObject) {
      return StatusCode::FAILURE;
    }
    trigBphysCollection().push_back(triggerObject);
    trigBphysMuonIndices.push_back(muonIndex);
    return StatusCode::SUCCESS;
  }
};


/**
 * @class TrigBmuxComboHypo
 * @brief EF hypothesis algorithm for B -> mu+ nu_mu X decays:
 *        B+ -> mu+ nu_mu anti-D0(-> K+ pi-)
 *        B0 -> mu+ nu_mu D*-(-> anti-D0(-> K+ pi-) pi-)
 *        B0 -> mu+ nu_mu D-(-> K+ pi- pi-)
 *        B_s0 -> mu+ nu_mu D_s-(->phi(-> K+ K-) pi-)
 *        anti-Lambda_b0 -> mu+ nu_mu anti-Lambda_c-(-> anti-p K+ pi-)
 */
class TrigBmuxComboHypo: public ::ComboHypo {
 public:
  TrigBmuxComboHypo(const std::string& name, ISvcLocator* pSvcLocator);
  TrigBmuxComboHypo() = delete;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;

  enum Decay : size_t {
    kD0,           // D0 -> K- pi+
    kDs,           // D_s+ -> K+ K- pi+
    kDplus,        // D+ -> K- pi+ pi+
    kLambdaC       // Lambda_c+ -> p K- pi+
  };

 private:
  StatusCode findBmuxCandidates(TrigBmuxState&) const;
  StatusCode createDecisionObjects(TrigBmuxState&) const;

  std::unique_ptr<xAOD::Vertex> fit(
      const EventContext& context,
      const std::vector<ElementLink<xAOD::TrackParticleContainer>>& trackParticleLinks,
      Decay decay) const;

  xAOD::TrigBphys* makeTriggerObject(
      const xAOD::Vertex& vertex,
      xAOD::TrigBphys::pType type,
      const std::vector<double>& trkMass) const;

  bool isIdenticalTracks(const xAOD::TrackParticle* lhs, const xAOD::TrackParticle* rhs) const;
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
  Gaudi::Property<size_t> m_fitAttemptsWarningThreshold {this,
    "FitAttemptsWarningThreshold", 200, "Events processing this many calls of the vertex fitter will generate a WARNING message (time-out protect)"};
  Gaudi::Property<size_t> m_fitAttemptsBreakThreshold {this,
    "FitAttemptsBreakThreshold", 1000, "Events processing this many calls of the vertex fitter will generate a second WARNING message and the loop over combinations will be terminated at this point (time-out protect)"};

  // B+ -> mu+ nu_mu anti-D0(-> K+ pi-) and B0 -> mu+ nu_mu D*-(-> anti-D0(-> K+ pi-) pi-)
  Gaudi::Property<bool> m_BToD0 {this,
    "BToD0", true, "switch on/off B+ -> mu+ nu_mu anti-D0(-> K+ pi-) decay"};
  Gaudi::Property<bool> m_BToD0_makeDstar {this,
    "BToD0_makeDstar", true, "switch on/off B0 -> mu+ nu_mu D*-(-> anti-D0(-> K+ pi-) pi-) decay"};
  Gaudi::Property<double> m_BToD0_minD0KaonPt {this,
    "BToD0_minD0KaonPt", 1000., "minimum pT of kaon track from D0"};
  Gaudi::Property<double> m_BToD0_minD0PionPt {this,
    "BToD0_minD0PionPt", 1000., "minimum pT of pion track from D0"};
  Gaudi::Property<double> m_BToD0_minDstarPionPt {this,
    "BToD0_minDstarPionPt", 1000., "minimum pT of pion track from D*-"};
  Gaudi::Property<double> m_BToD0_maxDstarPionZ0 {this,
    "BToD0_maxDstarPionZ0", 5., "maximum z0 impact parameter of the pion track from D*+ wrt the fitted D0 vertex; no preselection if negative"};
  Gaudi::Property<std::pair<double, double>> m_BToD0_massRange {this,
    "BToD0_massRange", {-1., 10000.}, "B+/B0 mass range"};
  Gaudi::Property<std::pair<double, double>> m_BToD0_D0MassRange {this,
    "BToD0_D0MassRange", {1750., 2000.}, "D0 mass range"};
  Gaudi::Property<std::pair<double, double>> m_BToD0_DstarMassRange {this,
    "BToD0_DstarMassRange", {-1., 2110.}, "D*+ mass range"};
  Gaudi::Property<float> m_BToD0_chi2 {this,
    "BToD0_chi2", 20., "maximum chi2 of the fitted D0 vertex"};

  // B0 -> mu+ nu_mu D-(-> K+ pi- pi-)
  Gaudi::Property<bool> m_BdToD {this,
    "BdToD", true, "switch on/off B0 -> mu+ nu_mu D-(-> K+ pi- pi-) decay"};
  Gaudi::Property<double> m_BdToD_minKaonPt {this,
    "BdToD_minKaonPt", 1000., "minimum pT of kaon track from D-"};
  Gaudi::Property<double> m_BdToD_minPionPt {this,
    "BdToD_minPionPt", 1000., "minimum pT of pion track from D-"};
  Gaudi::Property<std::pair<double, double>> m_BdToD_massRange {this,
    "BdToD_massRange", {-1., 10000.}, "B0 mass range"};
  Gaudi::Property<std::pair<double, double>> m_BdToD_DMassRange {this,
    "BdToD_DMassRange", {1750., 2000.}, "D+ mass range"};
  Gaudi::Property<float> m_BdToD_chi2 {this,
    "BdToD_chi2", 27., "maximum chi2 of the fitted D- vertex"};

  // B_s0 -> mu+ nu_mu D_s-(->phi(-> K+ K-) pi-)
  Gaudi::Property<bool> m_BsToDs {this,
    "BsToDs", true, "switch on/off B_s0 -> mu+ nu_mu D_s-(->phi(-> K+ K-) pi-) decay"};
  Gaudi::Property<double> m_BsToDs_minKaonPt {this,
    "BsToDs_minKaonPt", 1000., "minimum pT of kaon track from phi(1020)"};
  Gaudi::Property<double> m_BsToDs_minPionPt {this,
    "BsToDs_minPionPt", 1000., "minimum pT of pion track from D_s-"};
  Gaudi::Property<std::pair<double, double>> m_BsToDs_massRange {this,
    "BsToDs_massRange", {-1., 10000.}, "B_s0 mass range"};
  Gaudi::Property<std::pair<double, double>> m_BsToDs_phiMassRange {this,
    "BsToDs_phiMassRange", {940., 1100.}, "phi(1020) mass range"};
  Gaudi::Property<std::pair<double, double>> m_BsToDs_DsMassRange {this,
    "BsToDs_DsMassRange", {1850., 2100.}, "D_s- mass range"};
  Gaudi::Property<float> m_BsToDs_chi2 {this,
    "BsToDs_chi2", 27., "maximum chi2 of the fitted D_s- vertex"};

  // anti-Lambda_b0 -> mu+ nu_mu anti-Lambda_c-(-> anti-p K+ pi-)
  Gaudi::Property<bool> m_LambdaBToLambdaC {this,
    "LambdaBToLambdaC", true, "switch on/off Lambda_b0 -> mu+ nu_mu anti-Lambda_c-(-> anti-p K+ pi-) decay"};
  Gaudi::Property<double> m_LambdaBToLambdaC_minProtonPt {this,
    "LambdaBToLambdaC_minProtonPt", 2000., "minimum pT of proton track"};
  Gaudi::Property<double> m_LambdaBToLambdaC_minKaonPt {this,
    "LambdaBToLambdaC_minKaonPt", 1000., "minimum pT of kaon track"};
  Gaudi::Property<double> m_LambdaBToLambdaC_minPionPt {this,
    "LambdaBToLambdaC_minPionPt", 1000., "minimum pT of pion track"};
  Gaudi::Property<std::pair<double, double>> m_LambdaBToLambdaC_massRange {this,
    "LambdaBToLambdaC_massRange", {-1., 10000.}, "Lambda_b0 mass range"};
  Gaudi::Property<std::pair<double, double>> m_LambdaBToLambdaC_LambdaCMassRange {this,
    "LambdaBToLambdaC_LambdaCMassRange", {2190., 2390.}, "Lambda_c- mass range"};
  Gaudi::Property<float> m_LambdaBToLambdaC_chi2 {this,
    "LambdaBToLambdaC_chi2", 27., "maximum chi2 of the fitted Lambda_c- vertex"};

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

  const static std::vector<std::vector<double>> s_trkMass;
};

#endif  // TRIG_TrigBmuxComboHypo_H
