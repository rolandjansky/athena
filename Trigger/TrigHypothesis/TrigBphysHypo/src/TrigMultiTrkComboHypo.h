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

namespace Trk {
class IVKalState;
}


/**
 * @class TrigMultiTrkState
 * @brief State class for TrigMultiTrkComboHypo algorithm
 */

class TrigMultiTrkState: public ::ITrigBphysState {
 public:
  virtual ~TrigMultiTrkState();
  const EventContext* context;
  const TrigCompositeUtils::DecisionContainer* previousDecisions;
  TrigCompositeUtils::DecisionContainer* decisions;
  xAOD::TrigBphysContainer* trigBphysCollection;
  std::vector<ElementLink<xAOD::TrackParticleContainer>> tracks;
  int isEventAccepted;
  std::vector<std::vector<size_t>> trigBphysIndices;
  virtual ElementLinkVector<TrigCompositeUtils::DecisionContainer>& getDecisionLinks(size_t) =0 ;
  virtual TrigCompositeUtils::DecisionIDContainer& getDecisionID(size_t) =0 ;
};

template<typename CONTAINER>
class TrigMultiTrkStateCand : public TrigMultiTrkState{
 public:
  virtual ~TrigMultiTrkStateCand() = default;
  struct LEG {
    ElementLink<CONTAINER> link;
    ElementLinkVector<TrigCompositeUtils::DecisionContainer> decisionLinks;
    TrigCompositeUtils::DecisionIDContainer decisionIDs;
  };
  std::vector<LEG> LegList;
  virtual ElementLinkVector<TrigCompositeUtils::DecisionContainer>& getDecisionLinks(size_t i)  override 
  { return LegList.at(i).decisionLinks; }
  virtual TrigCompositeUtils::DecisionIDContainer& getDecisionID(size_t i)  override
  { return LegList.at(i).decisionIDs; }
};



class TrigMultiTrkComboHypo: public ::ComboHypo {
 public:
  TrigMultiTrkComboHypo(const std::string& name, ISvcLocator* pSvcLocator);
  TrigMultiTrkComboHypo() = delete;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;

 private:

  void              FillState(TrigMultiTrkState* state,
                       const EventContext* context,
                       const TrigCompositeUtils::DecisionContainer* previousDecisions,
                       TrigCompositeUtils::DecisionContainer* decisions,
                       xAOD::TrigBphysContainer* trigBphysCollection) const;

  template<typename T>
  StatusCode mergeFromDecisions(TrigMultiTrkStateCand<T>&) const;

  StatusCode mergeTracksFromViews(TrigMultiTrkState&) const;
  StatusCode mergeTracksFromDecisions(TrigMultiTrkState&) const;
  StatusCode filterTrackCombinations(TrigMultiTrkState&) const;
  StatusCode copyDecisionObjects(TrigMultiTrkState&) const;
  
  template<typename T>
  StatusCode findDiTrackCandidates(TrigMultiTrkStateCand<T>&) const;

  StatusCode createDecisionObjects(TrigMultiTrkState&) const;

  xAOD::Vertex* fit(const std::vector<ElementLink<xAOD::TrackParticleContainer>>& tracklist, Trk::IVKalState*) const;
  xAOD::TrigBphys* makeTrigBPhys(xAOD::Vertex* vertex, Trk::IVKalState* fitterState, const Amg::Vector3D& beamspot) const;
  bool isIdenticalTracks(const xAOD::TrackParticle* lhs, const xAOD::TrackParticle* rhs) const;
  bool isIdenticalTracks(const xAOD::Muon* lhs, const xAOD::Muon* rhs) const;
  bool isIdenticalTracks(const xAOD::Electron* lhs, const xAOD::Electron* rhs) const;
  bool isInMassRange(double mass) const;
  float Lxy(const xAOD::TrigBphys*, const Amg::Vector3D&) const;

  SG::ReadHandleKey<xAOD::TrackParticleContainer>
    m_trackParticleContainerKey {this, "TrackCollectionKey", "Tracks", "input TrackParticle container name"};

  SG::WriteHandleKey<xAOD::TrigBphysContainer>
    m_trigBphysContainerKey {this, "TrigBphysCollectionKey", "TrigBphysContainer", "output TrigBphysContainer name"};

  SG::ReadCondHandleKey<InDet::BeamSpotData>
    m_beamSpotKey {this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot"};

  Gaudi::Property<unsigned int> m_nTrk {this, "nTracks", 2, "number of tracks in the vertex"};
  Gaudi::Property<std::vector<double>> m_trkMass {this, "trackMasses", {105.6583745, 105.6583745}, "track masses for vertex reco (one per track)"};
  Gaudi::Property<std::vector<float>> m_trkPt {this, "trkPtThresholds", {3650., 3650.} ,"minimum track pTs (one per track, sorted descending)"};
  Gaudi::Property<std::vector<std::pair<double, double>>> m_massRange {this, "massRanges", { {0., 100000.} }, "mass ranges"};
  Gaudi::Property<float> m_deltaR {this, "deltaR", 0.01, "minimum deltaR between same-sign tracks (overlap removal)"};
  Gaudi::Property<float> m_chi2 {this, "chi2", 150., "chi2 cut for the fitted vertex"};

  Gaudi::Property<bool> m_isStreamer {this, "isStreamer", false, "if true we do not create trigger objects, just copy all appropriate decisions to the next step or break the chain"};
  Gaudi::Property<bool> m_doElectrons {this, "doElectrons", false, "use electrons instead of muons"};
  Gaudi::Property<std::string> m_trigLevel {this, "trigLevel", "EF", "trigger Level to set for created TrigBphys objects"};

  ToolHandle<InDet::VertexPointEstimator> m_vertexPointEstimator {this, "VertexPointEstimator", "", "tool to find starting point for the vertex fitter"};
  ToolHandle<Trk::TrkVKalVrtFitter> m_vertexFitter {this, "VertexFitter", "", "VKalVrtFitter tool to fit tracks into the common vertex"};

  ToolHandle<GenericMonitoringTool> m_monTool {this, "MonTool", "", "monitoring tool"};

  TrigCompositeUtils::DecisionIDContainer m_allowedIDs;
};

#endif  // TRIG_TrigMultiTrkComboHypo_H
