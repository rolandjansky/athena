/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class MuonCompetingClustersOnTrackCreator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for MuonCompetingClustersOnTrack object production
///////////////////////////////////////////////////////////////////
// Version 1.0 18/07/2004
///////////////////////////////////////////////////////////////////

#include "MuonCompetingClustersOnTrackCreator.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"

#include <list>
#include <vector>


namespace Muon {

  MuonCompetingClustersOnTrackCreator::MuonCompetingClustersOnTrackCreator
  (const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa)
  {
    // algtool interface - necessary!
    declareInterface<IMuonCompetingClustersOnTrackCreator>(this);
    declareInterface<Trk::ICompetingRIOsOnTrackTool>(this);
  }

  StatusCode MuonCompetingClustersOnTrackCreator::initialize()
  {

    ATH_MSG_VERBOSE("MuonCompetingClustersOnTrackCreator::Initializing");
    ATH_CHECK( m_clusterCreator.retrieve() );

    return StatusCode::SUCCESS;
  }
  
  const CompetingMuonClustersOnTrack* MuonCompetingClustersOnTrackCreator::createBroadCluster(const std::list< const Trk::PrepRawData * > & prds, const double errorScaleFactor ) const
  {
    if (prds.size() == 0) return 0;

// implement cluster formation
    std::vector <const Muon::MuonClusterOnTrack* >* rios = new std::vector <const Muon::MuonClusterOnTrack* >() ;
    std::vector < double >* assProbs = new std::vector < double >();
    std::list< const Trk::PrepRawData* >::const_iterator  it = prds.begin();
    std::list< const Trk::PrepRawData* >::const_iterator  it_end = prds.end();
    for( ;it!=it_end;++it ){
      Identifier id = (*it)->identify();
      const Trk::TrkDetElementBase* detEl = (*it)->detectorElement();
      const Amg::Vector3D gHitPos = detEl->center(id);
      const Muon::MuonClusterOnTrack* cluster = m_clusterCreator->createRIO_OnTrack( **it, gHitPos ); 
      rios->push_back( cluster );
      double prob = 1./(errorScaleFactor*errorScaleFactor);
      assProbs->push_back( prob );
    }
    const CompetingMuonClustersOnTrack* competingRio = new CompetingMuonClustersOnTrack( rios, assProbs );
    return competingRio; 
  }
}
