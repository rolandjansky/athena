/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "MuonCompetingClustersOnTrackCreator.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"

#include <list>
#include <vector>


namespace Muon {

  MuonCompetingClustersOnTrackCreator::MuonCompetingClustersOnTrackCreator
  (const std::string& ty,const std::string& na,const IInterface* pa)
    : AlgTool(ty,na,pa), m_clusterCreator("Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator")
  {
    // algtool interface - necessary!
    declareInterface<IMuonCompetingClustersOnTrackCreator>(this);
    declareInterface<Trk::ICompetingRIOsOnTrackTool>(this);
  }


  MuonCompetingClustersOnTrackCreator::~MuonCompetingClustersOnTrackCreator(){}


  StatusCode MuonCompetingClustersOnTrackCreator::initialize()
  {

    StatusCode sc = AlgTool::initialize(); 

    MsgStream log(msgSvc(),name());

    log << MSG::VERBOSE << "MuonCompetingClustersOnTrackCreator::Initializing" << endreq;
    sc = service("StoreGateSvc", m_storeGate);
    if (sc.isFailure()) {
      log << MSG::FATAL << "StoreGate service not found" << endreq;
      return StatusCode::FAILURE;
    }
    sc = m_clusterCreator.retrieve();
    if( sc.isSuccess() ){
      log<<MSG::INFO << "Retrieved " << m_clusterCreator << endreq;
    }else{
      log<<MSG::FATAL<<"Could not get " << m_clusterCreator <<endreq; 
    }

    return sc;
  }

  StatusCode MuonCompetingClustersOnTrackCreator::finalize()
  {
    StatusCode sc = AlgTool::finalize(); 
    return sc;
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
