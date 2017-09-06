/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDALGS_MUONCREATORALG_H
#define MUONCOMBINEDALGS_MUONCREATORALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>
#include "StoreGate/ReadHandleKey.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/SlowMuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "MuonSegment/MuonSegment.h"

namespace MuonCombined {
  class IMuonCreatorTool;
}

class MuonCreatorAlg : public AthAlgorithm
{
 public:
  MuonCreatorAlg(const std::string& name, ISvcLocator* pSvcLocator);

  ~MuonCreatorAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  StatusCode createAndRecord( xAOD::TrackParticleContainer*& xaod, TrackCollection*& tracks, std::string name ) const;
  StatusCode createAndRecordSegments( xAOD::MuonSegmentContainer*& xaodSegments,Trk::SegmentCollection*& segs, std::string name ) const;
  StatusCode retrieveOrCreateAndRecord( xAOD::CaloClusterContainer*& xaod ) const;

  ToolHandle<MuonCombined::IMuonCreatorTool> m_muonCreatorTool;
  std::string m_muonCollectionName;
  std::string m_slowMuonCollectionName;
  std::string m_combinedCollectionName;
  std::string m_extrapolatedCollectionName;
  std::string m_msOnlyExtrapolatedCollectionName;
  SG::ReadHandleKey<InDetCandidateCollection> m_indetCandidateCollectionName;
  std::string m_muonCandidateCollectionName;
  std::string m_segContainerName;
  std::string m_clusterContainerName;
  bool m_buildSlowMuon;

};


#endif
