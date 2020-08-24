/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigBphys/TrigBphysContainer.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "DecisionHandling/ComboHypo.h"

#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "TrigMultiTrkComboHypoTool.h"


class TrigMultiTrkComboHypo: public ::ComboHypo {
 public:
  TrigMultiTrkComboHypo(const std::string& name, ISvcLocator* pSvcLocator);
  TrigMultiTrkComboHypo() = delete;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;
  virtual StatusCode finalize() override;

 protected:
  // the vertexing tool is not reentrant so this algorithm cannot be used reentrantly
  bool isReEntrant() const override { return false; }

 private:
  StatusCode executeL2(const EventContext& context) const;
  StatusCode executeEF(const EventContext& context) const;
  xAOD::TrigBphys* fit(const std::vector<ElementLink<xAOD::TrackParticleContainer>>& tracklist) const;
  bool isIdenticalTracks(const xAOD::TrackParticle* lhs, const xAOD::TrackParticle* rhs) const;
  bool isInMassRange(double mass) const;

  SG::ReadHandleKey<xAOD::TrackParticleContainer>
    m_trackParticleContainerKey {this, "TrackCollectionKey", "Tracks", "input TrackParticle container name"};

  SG::ReadHandleKey<xAOD::MuonContainer>
    m_muonContainerKey {this, "MuonCollectionKey", "CBCombinedMuon", "input EF Muon container name"};

  SG::WriteHandleKey<xAOD::TrigBphysContainer>
    m_trigBphysContainerKey {this, "TrigBphysCollectionKey", "TrigBphysContainer", "output TrigBphysContainer name"};

  Gaudi::Property<unsigned int> m_nTrk {this, "nTracks", 2, "number of tracks in the vertex"};
  Gaudi::Property<std::vector<double>> m_trkMass {this, "trackMasses", {105.6583745, 105.6583745}, "track masses for vertex reco (one per track)"};
  Gaudi::Property<std::vector<float>> m_trkPt {this, "trkPtThresholds", {3650., 3650.} ,"minimum track pTs (one per track, sorted descending)"};
  Gaudi::Property<std::vector<std::pair<double, double>>> m_massRange {this, "massRanges", { {0., 100000.} }, "mass ranges"};
  Gaudi::Property<float> m_deltaR {this, "deltaR", 0.01, "minimum deltaR between same-sign tracks (overlap removal)"};

  Gaudi::Property<std::string> m_trigLevelString {this, "trigLevel", "", "trigger Level to set for created TrigBphys objects"};
  xAOD::TrigBphys::levelType m_trigLevel;

  ToolHandle<InDet::VertexPointEstimator> m_vertexPointEstimator {this, "VertexPointEstimator", "", "tool to find starting point for the vertex fitter"};
  ToolHandle<Trk::TrkVKalVrtFitter> m_vertexFitter {this, "VertexFitter", "", "VKalVrtFitter tool to fit tracks into the common vertex"};

  ToolHandle<GenericMonitoringTool> m_monTool {this, "MonTool", "", "monitoring tool"};

  TrigCompositeUtils::DecisionIDContainer m_allowedIDs;

};

#endif  // TRIG_TrigMultiTrkComboHypo_H
