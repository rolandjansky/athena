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
  declareProperty("BuildSlowMuon",m_buildSlowMuon=false);
  declareProperty("CreateSAmuons", m_doSA=false);

}

MuonCreatorAlg::~MuonCreatorAlg(){}

StatusCode MuonCreatorAlg::initialize()
{

  ATH_CHECK(m_muonCreatorTool.retrieve());
  ATH_CHECK(m_muonCollectionName.initialize());
  ATH_CHECK(m_slowMuonCollectionName.initialize());
  ATH_CHECK(m_indetCandidateCollectionName.initialize(!m_doSA));
  ATH_CHECK(m_muonCandidateCollectionName.initialize(!m_buildSlowMuon));
  m_segTrkContainerName = "Trk"+m_segContainerName.key();
  m_segContainerName = "xaod"+m_segContainerName.key();
  ATH_CHECK(m_segContainerName.initialize());
  ATH_CHECK(m_segTrkContainerName.initialize());
  m_combinedTrkCollectionName = m_combinedCollectionName.key()+"Tracks";
  m_combinedCollectionName = m_combinedCollectionName.key()+"TrackParticles";
  ATH_CHECK(m_combinedCollectionName.initialize());
  ATH_CHECK(m_combinedTrkCollectionName.initialize());
  m_extrapolatedTrkCollectionName = m_extrapolatedCollectionName.key()+"Tracks";
  m_extrapolatedCollectionName = m_extrapolatedCollectionName.key()+"TrackParticles";
  ATH_CHECK(m_extrapolatedCollectionName.initialize());
  ATH_CHECK(m_extrapolatedTrkCollectionName.initialize());
  m_msOnlyExtrapolatedTrkCollectionName = m_msOnlyExtrapolatedCollectionName.key()+"Tracks";
  m_msOnlyExtrapolatedCollectionName = m_msOnlyExtrapolatedCollectionName.key()+"TrackParticles";
  ATH_CHECK(m_msOnlyExtrapolatedCollectionName.initialize());
  ATH_CHECK(m_msOnlyExtrapolatedTrkCollectionName.initialize());
  if(m_clusterContainerName.key()!=""){
    ATH_CHECK(m_clusterContainerName.initialize());
    m_clusterContainerLinkName = m_clusterContainerName.key()+"_links";
    ATH_CHECK(m_clusterContainerLinkName.initialize());
  }

  return StatusCode::SUCCESS; 
}

StatusCode MuonCreatorAlg::execute()
{

  const InDetCandidateCollection *indetCandidateCollection = 0;
  if(!m_doSA){
    SG::ReadHandle<InDetCandidateCollection> indetRH(m_indetCandidateCollectionName);
    if(!indetRH.isValid()){
      ATH_MSG_ERROR("Could not read "<< m_indetCandidateCollectionName);
      return StatusCode::FAILURE;
    }
    else{
      indetCandidateCollection = indetRH.cptr();
    }
  }

  // Create the xAOD container and its auxiliary store:
  SG::WriteHandle<xAOD::MuonContainer> wh_muons(m_muonCollectionName);
  ATH_CHECK(wh_muons.record(std::make_unique<xAOD::MuonContainer>(), std::make_unique<xAOD::MuonAuxContainer>()));
  ATH_MSG_DEBUG( "Recorded Muons with key: " << m_muonCollectionName.key() );    
  
  MuonCombined::IMuonCreatorTool::OutputData output(*(wh_muons.ptr()));



  // Create the and record track particles:
  //combined tracks
  SG::WriteHandle<xAOD::TrackParticleContainer> wh_combtp(m_combinedCollectionName);
  SG::WriteHandle<TrackCollection> wh_combtrk(m_combinedTrkCollectionName);
  ATH_CHECK(wh_combtp.record(std::make_unique<xAOD::TrackParticleContainer>(), std::make_unique<xAOD::TrackParticleAuxContainer>()));
  ATH_CHECK(wh_combtrk.record(std::make_unique<TrackCollection>()));
  output.combinedTrackParticleContainer = wh_combtp.ptr();
  output.combinedTrackCollection = wh_combtrk.ptr();

  //extrapolated tracks
  SG::WriteHandle<xAOD::TrackParticleContainer> wh_extrtp(m_extrapolatedCollectionName);
  SG::WriteHandle<TrackCollection> wh_extrtrk(m_extrapolatedTrkCollectionName);
  ATH_CHECK(wh_extrtp.record(std::make_unique<xAOD::TrackParticleContainer>(), std::make_unique<xAOD::TrackParticleAuxContainer>()));
  ATH_CHECK(wh_extrtrk.record(std::make_unique<TrackCollection>()));
  output.extrapolatedTrackParticleContainer = wh_extrtp.ptr();
  output.extrapolatedTrackCollection = wh_extrtrk.ptr();

  //msonly tracks
  SG::WriteHandle<xAOD::TrackParticleContainer> wh_msextrtp(m_msOnlyExtrapolatedCollectionName);
  SG::WriteHandle<TrackCollection> wh_msextrtrk(m_msOnlyExtrapolatedTrkCollectionName);
  ATH_CHECK(wh_msextrtp.record(std::make_unique<xAOD::TrackParticleContainer>(), std::make_unique<xAOD::TrackParticleAuxContainer>()));
  ATH_CHECK(wh_msextrtrk.record(std::make_unique<TrackCollection>()));
  output.msOnlyExtrapolatedTrackParticleContainer = wh_msextrtp.ptr();
  output.msOnlyExtrapolatedTrackCollection = wh_msextrtrk.ptr();


  //segments
  SG::WriteHandle<xAOD::MuonSegmentContainer> wh_segment(m_segContainerName);
  ATH_CHECK(wh_segment.record(std::make_unique<xAOD::MuonSegmentContainer>(),std::make_unique<xAOD::MuonSegmentAuxContainer>()));
  output.xaodSegmentContainer=wh_segment.ptr();

  SG::WriteHandle<Trk::SegmentCollection> wh_segmentTrk(m_segTrkContainerName);
  ATH_CHECK(wh_segmentTrk.record(std::make_unique<Trk::SegmentCollection>()));
  output.muonSegmentCollection=wh_segmentTrk.ptr();

  xAOD::SlowMuonContainer *slowMuons=0;
  xAOD::SlowMuonAuxContainer *slowMuonsAux=0;
  if (m_buildSlowMuon){
    slowMuons = new xAOD::SlowMuonContainer(); 
    slowMuonsAux = new xAOD::SlowMuonAuxContainer();
    slowMuons->setStore(slowMuonsAux);
    output.slowMuonContainer = slowMuons;    
  }

  const MuonCandidateCollection *muonCandidateCollection =0;

  if(!m_buildSlowMuon){
    SG::ReadHandle<MuonCandidateCollection> muonCandidateRH(m_muonCandidateCollectionName);
    if(!muonCandidateRH.isValid()){
      ATH_MSG_ERROR("Could not read "<< m_muonCandidateCollectionName);
      return StatusCode::FAILURE;
    }
    muonCandidateCollection = muonCandidateRH.cptr();
  }

  
  // calo clusters
  if( m_clusterContainerName.key() != "" ){
    xAOD::CaloClusterContainer *caloclusters = new xAOD::CaloClusterContainer();
    xAOD::CaloClusterAuxContainer *caloclustersaux = new xAOD::CaloClusterAuxContainer();
    caloclusters->setStore(caloclustersaux);
    output.clusterContainer = caloclusters;
    CaloClusterCellLinkContainer *clusterlinks = new CaloClusterCellLinkContainer();
    SG::WriteHandle<xAOD::CaloClusterContainer> wh_clusters(m_clusterContainerName);
    SG::WriteHandle<CaloClusterCellLinkContainer> wh_clusterslink(m_clusterContainerLinkName);
    m_muonCreatorTool->create(muonCandidateCollection, indetCandidateCollection,output);

    //record clusters and set the links
    ATH_CHECK(wh_clusters.record(std::unique_ptr<xAOD::CaloClusterContainer>(caloclusters),std::unique_ptr<xAOD::CaloClusterAuxContainer>(caloclustersaux)));
    auto sg = wh_clusters.storeHandle().get();
    for (xAOD::CaloCluster* cl : *caloclusters) {
      cl->setLink(clusterlinks, sg);
    }
    ATH_CHECK(wh_clusterslink.record(std::unique_ptr<CaloClusterCellLinkContainer>(clusterlinks)));


  }  
  else{

    // build muons
    if(!(muonCandidateCollection)){
      ATH_MSG_WARNING("candidate collection missing, skip muon creation");
    }
    else m_muonCreatorTool->create(muonCandidateCollection, indetCandidateCollection ,output);
  }

  if (m_buildSlowMuon){
    SG::WriteHandle<xAOD::SlowMuonContainer> wh_slowmuon(m_slowMuonCollectionName);
    ATH_CHECK(wh_slowmuon.record(std::unique_ptr<xAOD::SlowMuonContainer>(slowMuons),std::unique_ptr<xAOD::SlowMuonAuxContainer>(slowMuonsAux)));
  }

  return StatusCode::SUCCESS;
}


StatusCode MuonCreatorAlg::finalize()
{
  return StatusCode::SUCCESS;
}
