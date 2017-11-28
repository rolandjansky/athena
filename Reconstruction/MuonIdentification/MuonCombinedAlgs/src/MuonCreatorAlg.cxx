/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCreatorAlg.h"
#include "MuonCombinedToolInterfaces/IMuonCreatorTool.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/MuonSegmentAuxContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "TrkSegment/SegmentCollection.h"
#include "MuonSegment/MuonSegment.h"
#include "xAODMuon/SlowMuonContainer.h"
#include "xAODMuon/SlowMuonAuxContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

MuonCreatorAlg::MuonCreatorAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_muonCreatorTool("MuonCombined::MuonCreatorTool/MuonCreatorTool")
{  
  declareProperty("MuonCreatorTool",m_muonCreatorTool);
  declareProperty("MuonContainerLocation",m_muonCollectionName = "Muons" );
  declareProperty("SlowMuonContainerLocation",m_slowMuonCollectionName = "SlowMuons" );
  declareProperty("CombinedLocation", m_combinedCollectionName = "CombinedMuon" );
  declareProperty("ExtrapolatedLocation", m_extrapolatedCollectionName = "ExtrapolatedMuon" );
  declareProperty("MSOnlyExtrapolatedLocation", m_msOnlyExtrapolatedCollectionName = "MSOnlyExtrapolatedMuon" );
  declareProperty("SegmentContainerName", m_segContainerName = "MuonSegments" );
  declareProperty("BuildSlowMuon",m_buildSlowMuon=false);
  declareProperty("ClusterContainerName",m_clusterContainerName="MuonClusterCollection");
}

MuonCreatorAlg::~MuonCreatorAlg(){}

StatusCode MuonCreatorAlg::initialize()
{
  ATH_CHECK(m_muonCreatorTool.retrieve());
  ATH_CHECK(m_indetCandidateCollectionName.initialize());
  ATH_CHECK(m_muonCandidateCollectionName.initialize(!m_buildSlowMuon));

  return StatusCode::SUCCESS; 
}

StatusCode MuonCreatorAlg::execute()
{

  SG::ReadHandle<InDetCandidateCollection> indetCandidateCollection(m_indetCandidateCollectionName);
  if(!indetCandidateCollection.isValid()){
    ATH_MSG_ERROR("Could not read "<< m_indetCandidateCollectionName);
    return StatusCode::FAILURE;
  }

  // Create the xAOD container and its auxiliary store:
  xAOD::MuonContainer* xaod = new xAOD::MuonContainer();
  ATH_CHECK( evtStore()->record( xaod, m_muonCollectionName ) );

  xAOD::MuonAuxContainer* aux = new xAOD::MuonAuxContainer();
  ATH_CHECK( evtStore()->record( aux, m_muonCollectionName + "Aux." ) );
  xaod->setStore( aux );
  ATH_MSG_DEBUG( "Recorded Muons with key: " << m_muonCollectionName );    

  MuonCombined::IMuonCreatorTool::OutputData output(*xaod);

  // combined tracks
  ATH_CHECK(createAndRecord(output.combinedTrackParticleContainer,output.combinedTrackCollection,m_combinedCollectionName));

  // extrapolated tracks
  ATH_CHECK(createAndRecord(output.extrapolatedTrackParticleContainer,output.extrapolatedTrackCollection,m_extrapolatedCollectionName));

  //MS-only extrapolated tracks
  ATH_CHECK(createAndRecord(output.msOnlyExtrapolatedTrackParticleContainer,output.msOnlyExtrapolatedTrackCollection,m_msOnlyExtrapolatedCollectionName));

  // segments
  ATH_CHECK(createAndRecordSegments(output.xaodSegmentContainer,output.muonSegmentCollection,m_segContainerName));

  // calo clusters
  if( m_clusterContainerName != "" ) ATH_CHECK(retrieveOrCreateAndRecord(output.clusterContainer));
  
  if (m_buildSlowMuon){
    // Create the xAOD slow muon container and its auxiliary store:
    output.slowMuonContainer = new xAOD::SlowMuonContainer();
    ATH_CHECK( evtStore()->record( output.slowMuonContainer, m_slowMuonCollectionName ) );
    
    xAOD::SlowMuonAuxContainer* auxSlowMuon = new xAOD::SlowMuonAuxContainer();
    ATH_CHECK( evtStore()->record( auxSlowMuon, m_slowMuonCollectionName + "Aux." ) );
    output.slowMuonContainer->setStore( auxSlowMuon );
    ATH_MSG_DEBUG( "Recorded Slow Muons with key: " << m_slowMuonCollectionName );
    const MuonCandidateCollection* noMuons=0;
    m_muonCreatorTool->create(noMuons, indetCandidateCollection.cptr() ,output);
  }
  else{
    SG::ReadHandle<MuonCandidateCollection> muonCandidateCollection(m_muonCandidateCollectionName);
    if(!muonCandidateCollection.isValid()){
      ATH_MSG_ERROR("Could not read "<< m_muonCandidateCollectionName);
      return StatusCode::FAILURE;
    }

    // build muons
    if(!(muonCandidateCollection.cptr())){
      ATH_MSG_WARNING("candidate collection missing, skip muon creation");
    }
    else m_muonCreatorTool->create(muonCandidateCollection.cptr(), indetCandidateCollection.cptr() ,output);
  }

  return StatusCode::SUCCESS;
}

StatusCode MuonCreatorAlg::retrieveOrCreateAndRecord( xAOD::CaloClusterContainer*& xaod ) const {

  // try retrieving the container
  if(evtStore()->contains<xAOD::CaloClusterContainer>(m_clusterContainerName)) {
    if(evtStore()->retrieve(xaod,m_clusterContainerName).isFailure()) {
      ATH_MSG_FATAL( "Unable to retrieve " << m_clusterContainerName );
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Retrieved CaloClusterContainer with key: " << m_clusterContainerName );    
  }else{
    // if not found in SG, create it
    xaod = new xAOD::CaloClusterContainer();
    ATH_CHECK( evtStore()->record( xaod, m_clusterContainerName ) );
	
    xAOD::CaloClusterAuxContainer* aux = new xAOD::CaloClusterAuxContainer();
    ATH_CHECK( evtStore()->record( aux, m_clusterContainerName + "Aux." ) );
    xaod->setStore( aux );
    ATH_MSG_DEBUG( "Recorded CaloClusterContainer with key: " << m_clusterContainerName );    
  }
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
