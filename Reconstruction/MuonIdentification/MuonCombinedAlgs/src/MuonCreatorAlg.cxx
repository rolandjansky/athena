/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCreatorAlg.h"
#include "MuonCombinedToolInterfaces/IMuonCreatorTool.h"

#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/MuonSegmentAuxContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "TrkSegment/SegmentCollection.h"
#include "MuonSegment/MuonSegment.h"

MuonCreatorAlg::MuonCreatorAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_muonCreatorTool("MuonCombined::MuonCreatorTool/MuonCreatorTool")
{  
  declareProperty("MuonCreatorTool",m_muonCreatorTool);
  declareProperty("MuonContainerLocation",m_muonCollectionName = "Muons" );
  declareProperty("CombinedLocation", m_combinedCollectionName = "CombinedMuon" );
  declareProperty("ExtrapolatedLocation", m_extrapolatedCollectionName = "ExtrapolatedMuon" );
  declareProperty("InDetCandidateLocation",m_indetCandidateCollectionName = "InDetCandidates" );
  declareProperty("MuonCandidateLocation", m_muonCandidateCollectionName = "MuonCandidates" );
  declareProperty("SegmentContainerName", m_segContainerName = "MuonSegments" );
}

MuonCreatorAlg::~MuonCreatorAlg()
{

}

StatusCode MuonCreatorAlg::initialize()
{
  ATH_CHECK(m_muonCreatorTool.retrieve());

  return StatusCode::SUCCESS; 
}

StatusCode MuonCreatorAlg::execute()
{

  InDetCandidateCollection* indetCandidateCollection = 0;
  if(evtStore()->contains<InDetCandidateCollection>(m_indetCandidateCollectionName)) {
    if(evtStore()->retrieve(indetCandidateCollection,m_indetCandidateCollectionName).isFailure()) {
      ATH_MSG_FATAL( "Unable to retrieve " << m_indetCandidateCollectionName );
      return StatusCode::FAILURE;
    }
  }
  
  MuonCandidateCollection* muonCandidateCollection = 0;
  if(evtStore()->contains<MuonCandidateCollection>(m_muonCandidateCollectionName)) {
    if(evtStore()->retrieve(muonCandidateCollection,m_muonCandidateCollectionName).isFailure()) {
      ATH_MSG_FATAL( "Unable to retrieve " << m_muonCandidateCollectionName );
      return StatusCode::FAILURE;
    }
  }
  
  // Create the xAOD container and its auxiliary store:
  xAOD::MuonContainer* xaod = new xAOD::MuonContainer();
  ATH_CHECK( evtStore()->record( xaod, m_muonCollectionName ) );

  xAOD::MuonAuxContainer* aux = new xAOD::MuonAuxContainer();
  ATH_CHECK( evtStore()->record( aux, m_muonCollectionName + "Aux." ) );
  xaod->setStore( aux );
  ATH_MSG_DEBUG( "Recorded Muons with key: " << m_muonCollectionName );    

  xAOD::TrackParticleContainer* combinedTP = 0;
  TrackCollection* combinedTracks = 0;
  ATH_CHECK(createAndRecord(combinedTP,combinedTracks,m_combinedCollectionName));

  xAOD::TrackParticleContainer* extrapolatedTP = 0;
  TrackCollection* extrapolatedTracks = 0;
  ATH_CHECK(createAndRecord(extrapolatedTP,extrapolatedTracks,m_extrapolatedCollectionName));

  xAOD::MuonSegmentContainer* xaodSegments = 0;
  Trk::SegmentCollection* segments = 0;
  ATH_CHECK(createAndRecordSegments(xaodSegments,segments,m_segContainerName));

  m_muonCreatorTool->create(muonCandidateCollection,indetCandidateCollection,*xaod,combinedTP,combinedTracks,extrapolatedTP,extrapolatedTracks,xaodSegments, segments);
  

  return StatusCode::SUCCESS;
}

StatusCode MuonCreatorAlg::createAndRecord( xAOD::TrackParticleContainer*& xaod, TrackCollection*& tracks, std::string name ) const {
  std::string tpName = name + "TrackParticles";
  std::string trackName = name + "Tracks";

  // Create the xAOD container and its auxiliary store:
  xaod = new xAOD::TrackParticleContainer();
  ATH_CHECK( evtStore()->record( xaod, tpName ) );
  xAOD::TrackParticleAuxContainer* aux = new xAOD::TrackParticleAuxContainer();
  ATH_CHECK( evtStore()->record( aux, tpName + "Aux." ) );
  xaod->setStore( aux );
  ATH_MSG_DEBUG( "Recorded TrackParticles with key: " << tpName );

  tracks = new TrackCollection();
  return evtStore()->record( tracks, trackName );
}

StatusCode MuonCreatorAlg::createAndRecordSegments( xAOD::MuonSegmentContainer*& xaodSegments, Trk::SegmentCollection*& segments, std::string name ) const {
  
  std::string xName = "xaod" + name;
  std::string trkName = "Trk" + name;

  // Create the xAOD container and its auxiliary store:
  xaodSegments = new xAOD::MuonSegmentContainer();
  ATH_CHECK( evtStore()->record( xaodSegments, xName ) );
  xAOD::MuonSegmentAuxContainer* aux = new xAOD::MuonSegmentAuxContainer();
  ATH_CHECK( evtStore()->record( aux, xName + "Aux." ) );

  xaodSegments->setStore( aux);
  ATH_MSG_DEBUG( "Recorded MuonSegments with key: " << xName);

  segments = new Trk::SegmentCollection();
  return evtStore()->record( segments, trkName );  
}

StatusCode MuonCreatorAlg::finalize()
{
  return StatusCode::SUCCESS;
}
