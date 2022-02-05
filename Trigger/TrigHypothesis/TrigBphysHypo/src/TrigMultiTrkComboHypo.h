/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigMultiTrkComboHypo.h
 **
 **   Description: multi-track hypothesis algorithm
 **
 **   Author: Heather Russell
 **
 **************************************************************************/

#ifndef TRIG_TrigMultiTrkComboHypo_H
#define TRIG_TrigMultiTrkComboHypo_H

#include <string>
#include <vector>

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "DecisionHandling/ComboHypo.h"

#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "ITrigBphysState.h"
#include "TrigMultiTrkComboHypoTool.h"
#include "BeamSpotConditionsData/BeamSpotData.h"

#include "AthViews/View.h"

namespace Trk {
class IVKalState;
}


/**
 * @class TrigMultiTrkState
 * @brief State class for TrigMultiTrkComboHypo algorithm
 */

class TrigMultiTrkStateBase: public ::ITrigBphysState {
 public:
  TrigMultiTrkStateBase() = delete;
  TrigMultiTrkStateBase(const EventContext& context,
                        const TrigCompositeUtils::DecisionContainer& previousDecisions,
                        TrigCompositeUtils::DecisionContainer& decisions,
                        xAOD::TrigBphysContainer* trigBphysCollection = nullptr,
                        const InDet::BeamSpotData* beamSpotData = nullptr)
      : ITrigBphysState(context, previousDecisions, decisions, trigBphysCollection, beamSpotData),
        m_isEventAccepted(0) {}
  virtual ~TrigMultiTrkStateBase() = default;
  virtual std::vector<ElementLink<TrigCompositeUtils::DecisionContainer>>& getDecisionLinks(size_t) = 0;
  virtual TrigCompositeUtils::DecisionIDContainer& getDecisionIDs(size_t) = 0;
  virtual void addTrigBphysObject(xAOD::TrigBphys*, const std::vector<size_t>& legIndices) = 0;

  void setEventAccepted(bool flag = true) { m_isEventAccepted = (flag ? 1 : 0); }
  inline int isEventAccepted() const { return m_isEventAccepted; }
  inline std::vector<std::vector<size_t>>& trigBphysLegIndices() { return m_trigBphysLegIndices; }
  inline std::vector<size_t>& getTrigBphysLegIndices(size_t idx) { return m_trigBphysLegIndices[idx]; }
  inline std::vector<ElementLink<xAOD::TrackParticleContainer>>& tracks() { return m_tracks; }

 private:
  int m_isEventAccepted;
  std::vector<std::vector<size_t>> m_trigBphysLegIndices;
  std::vector<ElementLink<xAOD::TrackParticleContainer>> m_tracks;
};


template<typename CONTAINER>
class TrigMultiTrkState : public TrigMultiTrkStateBase {
 public:
  TrigMultiTrkState() = delete;
  TrigMultiTrkState(const EventContext& context,
                    const TrigCompositeUtils::DecisionContainer& previousDecisions,
                    TrigCompositeUtils::DecisionContainer& decisions,
                    xAOD::TrigBphysContainer* trigBphysCollection = nullptr,
                    const InDet::BeamSpotData* beamSpotData = nullptr)
      : TrigMultiTrkStateBase(context, previousDecisions, decisions, trigBphysCollection, beamSpotData) {}
  virtual ~TrigMultiTrkState() = default;

  struct LEPTON {
    ElementLink<CONTAINER> link;
    std::vector<ElementLink<TrigCompositeUtils::DecisionContainer>> decisionLinks;
    TrigCompositeUtils::DecisionIDContainer decisionIDs;
  };

  std::vector<LEPTON>& leptons() { return m_leptons; }
  virtual std::vector<ElementLink<TrigCompositeUtils::DecisionContainer>>& getDecisionLinks(size_t i) override final {
    return m_leptons.at(i).decisionLinks;
  }
  virtual TrigCompositeUtils::DecisionIDContainer& getDecisionIDs(size_t i) override final {
    return m_leptons.at(i).decisionIDs;
  }
  virtual void addTrigBphysObject(xAOD::TrigBphys* trigBphysObject, const std::vector<size_t>& legIndices) override final {
    trigBphysCollection().push_back(trigBphysObject);
    trigBphysLegIndices().push_back(legIndices);
  }

 private:
  std::vector<LEPTON> m_leptons;
};



class TrigMultiTrkComboHypo: public ::ComboHypo {
 public:
  TrigMultiTrkComboHypo(const std::string& name, ISvcLocator* pSvcLocator);
  TrigMultiTrkComboHypo() = delete;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;

 private:

  /**
   * @brief Go through state.previousDecisions() and fetch xAOD::TrackParticle objects associated with the nearest SG::View.
   * Enable overlap removal to get collection of unique objects at state.tracks().
   */
  StatusCode mergeTracksFromViews(TrigMultiTrkStateBase&) const;

  /**
   * @brief Go through state.previousDecisions(), fetch xAOD::Muons/xAOD::Electron objects attached to decisions
   * and save links to the their xAOD::TrackParticle objects in state.tracks().
   */
  template<typename CONTAINER>
  StatusCode mergeTracksFromDecisions(TrigMultiTrkStateBase&) const;

  /**
   * @brief Go through state.previousDecisions(), fetch xAOD::Muons/xAOD::Electron objects attached to decisions
   * and save links to them in state.leptons().
   */
  template<typename CONTAINER>
  StatusCode mergeLeptonsFromDecisions(TrigMultiTrkState<CONTAINER>&) const;

  /**
   * @brief Make all possible combinations from state.leptons(), fit tracks to the common vertex,
   * create xAOD::TrigBphys objects and put them into state.trigBphysCollection()
   *
   * combination is a selection of items from a collection, such that the order of items does not matter
   * leptons/tracks have already been sorted over pT, the algorithm will keep this order in created subsets of tracks, i.e.
   * for nTrk = 2 combinations are [0, 1], [0, 2], ..., [1, 2], [1, 3], ...
   * for nTrk = 3 combinations are [0, 1, 2], [0, 1, 3], ..., [0, 2, 3], [0, 2, 4], ..., [1, 2, 3], [1, 2, 4], ...
   */
  template<typename CONTAINER>
  StatusCode findMultiLeptonCandidates(TrigMultiTrkState<CONTAINER>&) const;

  /**
   * @brief Make all possible combinations from state.tracks(), fit tracks to the common vertex
   * and set state.isEventAccepted to true if at least one good combination is found;
   * no xAOD::TrigBphys objects will be created.
   */
  StatusCode filterTrackCombinations(TrigMultiTrkStateBase&) const;

  /**
   * @brief Make all possible combinations from electrons originating from the same BPH-0DR3-EM7J15 cluster,
   * only one track should pass e5_lhvloose requirements;
   * initialRoI word will be saved into xAOD::TrigBphys objects.
   */
  StatusCode processMergedElectrons(TrigMultiTrkState<xAOD::ElectronContainer>&) const;

  /**
   * @brief Build J/psi candidates from muon from SG::View and tracks from the same SG::View,
   * to be used for Tag-and-Probe PEB chains (similar to HLT_mu6_bJpsimutrk_MuonTrkPEB_L1MU5VF)
   */
  StatusCode findMuTrkCandidates(TrigMultiTrkState<xAOD::MuonContainer>&) const;

  /**
   * @brief All appropriate decisions from state.previousDecisions() will be copied to state.decisions()
   * if flag state.isEventAccepted() is set by filterTrackCombinations() method;
   * to be used only in Streamer mode.
   */
  StatusCode copyDecisionObjects(TrigMultiTrkStateBase&) const;

  /**
   * @brief For chains from CombinedSlice (similar to 'HLT_e9_lhvloose_e5_lhvloose_bBeeM6000_mu4_L1BPH-0M9-EM7-EM5_MU6')
   * we have decisionsInput().size() > 1, so that we should copy decisions created by EmptySteps from muon part.
   */
  StatusCode copyAdditionalDecisionObjects(TrigMultiTrkStateBase&) const;

  /**
   * @brief Create a decision for each xAOD::TrigBphys object from state.trigBphysCollection() and save it to state.decisions();
   * use hypoTools() to assign correct decisionIDs, not compatible with Streamer mode.
   */
  StatusCode createDecisionObjects(TrigMultiTrkStateBase&) const;


  std::unique_ptr<xAOD::Vertex> fit(
      const std::vector<ElementLink<xAOD::TrackParticleContainer>>& trackParticleLinks,
      const std::vector<double>& particleMasses,
      Trk::IVKalState& fitterState) const;

  xAOD::TrigBphys* makeTrigBPhys(
      const xAOD::Vertex& vertex,
      const std::vector<double>& particleMasses,
      const Amg::Vector3D& beamSpot,
      const Trk::IVKalState& fitterState) const;

  bool isIdenticalTracks(const xAOD::TrackParticle* lhs, const xAOD::TrackParticle* rhs) const;
  bool isIdenticalTracks(const xAOD::Muon* lhs, const xAOD::Muon* rhs) const;
  bool isIdenticalTracks(const xAOD::Electron* lhs, const xAOD::Electron* rhs) const;
  bool isInMassRange(double mass, size_t idx) const;
  float Lxy(const xAOD::TrigBphys& vertex, const Amg::Vector3D& beamSpot) const;
  bool passedDeltaRcut(const std::vector<xAOD::TrackParticle::GenVecFourMom_t>& momenta) const;

  SG::ReadHandleKey<xAOD::TrackParticleContainer>
    m_trackParticleContainerKey {this, "TrackCollectionKey", "Tracks", "input TrackParticle container name"};

  SG::WriteHandleKey<xAOD::TrigBphysContainer>
    m_trigBphysContainerKey {this, "TrigBphysCollectionKey", "TrigBphysContainer", "output TrigBphysContainer name"};

  SG::ReadCondHandleKey<InDet::BeamSpotData>
    m_beamSpotKey {this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot"};

  Gaudi::Property<std::vector<unsigned int>> m_nTrk {this, "nTracks", {2},
    "number of tracks to be fitted into the common vertex"};
  Gaudi::Property<std::vector<int>> m_nTrkCharge {this, "totalCharge", {},
    "magnitude of the total charge to accept, negative is none"};
  Gaudi::Property<std::vector<std::vector<double>>> m_trkMass {this, "trackMasses", {},
    "track masses for vertex reco (one per track); muon mass is used by default"};
  Gaudi::Property<std::vector<std::vector<double>>> m_trkPt {this, "trackPtThresholds", { {3650., 3650.} },
    "minimum track transverse momenta (one per track, sorted descending)"};
  Gaudi::Property<std::vector<std::pair<double, double>>> m_massRange {this, "massRange", { {0., 100000.} },
    "range of the invariant mass of the track combinations"};
  Gaudi::Property<bool> m_applyOverlapRemoval {this, "applyOverlapRemoval", true,
    "apply overlap removal for the close-by same-sign objects from different views"};
  Gaudi::Property<bool> m_combineInputDecisionCollections {this, "combineInputDecisionCollections", false,
    "combine objects attached to decisions from different input collections, needed for HLT_mu4_ivarloose_mu4_b10invmAB120vtx20_L12MU3V chains"};
  Gaudi::Property<bool> m_useLeptonMomentum {this, "useLeptonMomentum", false,
    "use 4-momentum of the xAOD::Muon to make fast calculation of the xAOD::TrigBphys mass, needed for consistency with TrigComboHypoTool::compute()"};
  Gaudi::Property<float> m_deltaR {this, "deltaR", 0.01,
    "minimum deltaR between same-sign tracks (overlap removal)"};
  Gaudi::Property<float> m_deltaRMax {this, "deltaRMax", std::numeric_limits<float>::max(),
    "maximum deltaR between tracks in a candidate"};
  Gaudi::Property<float> m_deltaRMin {this, "deltaRMin", std::numeric_limits<float>::lowest(),
    "maximum deltaR between tracks in a candidate"};
  Gaudi::Property<float> m_chi2 {this, "chi2", 150.,
    "chi2 cut for the fitted vertex"};
  Gaudi::Property<bool> m_isStreamer {this, "isStreamer", false,
    "if true we do not create trigger objects, just copy all appropriate decisions to the next step or break the chain"};
  Gaudi::Property<bool> m_isMuTrkMode {this, "isMuTrkMode", false,
    "make pairs between muon from SG::View and tracks from the same SG::View"};
  Gaudi::Property<bool> m_doElectrons {this, "doElectrons", false,
    "use electrons if true, otherwise use muons"};
  Gaudi::Property<std::string> m_trigLevel {this, "trigLevel", "EF",
    "trigger Level to set for created TrigBphys objects: L2, L2IO, L2MT or EF"};
  Gaudi::Property<std::vector<std::string>> m_mergedElectronChains {this, "mergedElectronChains", {"BPH-0DR3-EM7J15"},
    "patterns for BPH-0DR3-EM7J15 like chains"};

  ToolHandle<InDet::VertexPointEstimator> m_vertexPointEstimator {this, "VertexPointEstimator", "", "tool to find starting point for the vertex fitter"};
  ToolHandle<Trk::TrkVKalVrtFitter> m_vertexFitter {this, "VertexFitter", "", "VKalVrtFitter tool to fit tracks into the common vertex"};

  ToolHandle<GenericMonitoringTool> m_monTool {this, "MonTool", "", "monitoring tool"};

  TrigCompositeUtils::DecisionIDContainer m_allowedIDs;
  TrigCompositeUtils::DecisionIDContainer m_resolvedElectronIDs;
  TrigCompositeUtils::DecisionIDContainer m_mergedElectronIDs;

  double m_trkPtMin;

};

#endif  // TRIG_TrigMultiTrkComboHypo_H
