/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDALGS_MUONCREATORALG_H
#define MUONCOMBINEDALGS_MUONCREATORALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKeyArray.h"

#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "MuonCombinedEvent/InDetCandidateToTagMap.h"

#include "MuonCombinedToolInterfaces/IMuonCreatorTool.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/SlowMuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "MuonSegment/MuonSegment.h"

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include <string>

class MuonCreatorAlg : public AthAlgorithm
{
 public:
  MuonCreatorAlg(const std::string& name, ISvcLocator* pSvcLocator);

  ~MuonCreatorAlg()=default;

  StatusCode initialize();
  StatusCode execute();

 private:
  ToolHandle<MuonCombined::IMuonCreatorTool> m_muonCreatorTool{this,"MuonCreatorTool","MuonCombined::MuonCreatorTool/MuonCreatorTool","Muon creator tool"};
  SG::WriteHandleKey<xAOD::MuonContainer> m_muonCollectionName{this,"MuonContainerLocation", "Muons", "Muon Container"};
  SG::WriteHandleKey<xAOD::SlowMuonContainer> m_slowMuonCollectionName{this, "SlowMuonContainerLocation", "SlowMuons", "Slow Muon Container"};
  SG::WriteHandleKey<xAOD::TrackParticleContainer> m_combinedCollectionName{this, "CombinedLocation", "CombinedMuon", "Combined muons"};
  SG::WriteHandleKey<TrackCollection> m_combinedTrkCollectionName{this,"CombinedTrackLocation","CombinedMuon","Combined muon tracks"};
  SG::WriteHandleKey<xAOD::TrackParticleContainer> m_extrapolatedCollectionName{this, "ExtrapolatedLocation", "ExtrapolatedMuon", "Extrapolated muons"};
  SG::WriteHandleKey<TrackCollection> m_extrapolatedTrkCollectionName{this,"ExtrapolatedTrackLocation","ExtrapolatedMuon","Extrapolated muon tracks"};
  SG::WriteHandleKey<xAOD::TrackParticleContainer> m_msOnlyExtrapolatedCollectionName{this, "MSOnlyExtrapolatedLocation", "MSOnlyExtrapolatedMuon", "MS Extrapolated muons"};
  SG::WriteHandleKey<TrackCollection> m_msOnlyExtrapolatedTrkCollectionName{this,"MSOnlyExtrapolatedTrackLocation","MSOnlyExtrapolatedMuonTracks","MS extrapolated muon tracks"};
  SG::ReadHandleKey<InDetCandidateCollection> m_indetCandidateCollectionName{this,"InDetCandidateLocation","InDetCandidates","ID candidates"};
  SG::ReadHandleKey<MuonCandidateCollection> m_muonCandidateCollectionName{this,"MuonCandidateLocation","MuonCandidates","Muon candidates"};
  SG::ReadHandleKeyArray<MuonCombined::InDetCandidateToTagMap> m_tagMaps{this,"TagMaps",{"muidcoTagMap","stacoTagMap","muGirlTagMap","caloTagMap","segmentTagMap"},"ID candidate to tag maps"};
  SG::WriteHandleKey<xAOD::MuonSegmentContainer> m_segContainerName{this, "SegmentContainerName", "MuonSegments", "Segments"};
  SG::WriteHandleKey<Trk::SegmentCollection> m_segTrkContainerName{this,"TrackSegmentContainerName","MuonSegments","Track segments"};
  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_clusterContainerName{this, "ClusterContainerName", "MuonClusterCollection", "Clusters"};
  SG::WriteHandleKey<CaloClusterCellLinkContainer> m_clusterContainerLinkName{this,"CaloClusterCellLinkName","MuonClusterCollection","Cluster links"};

  Gaudi::Property<bool> m_buildSlowMuon{this, "BuildSlowMuon", false};
  Gaudi::Property<bool> m_doSA{this, "CreateSAmuons", false};
  Gaudi::Property<bool> m_makeClusters{this, "MakeClusters", true};

  // Monitoring tool
  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };

};


#endif
