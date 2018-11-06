/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
#include <vector>

MuonCreatorAlg::MuonCreatorAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_muonCreatorTool("MuonCombined::MuonCreatorTool/MuonCreatorTool")
{  
  declareProperty("MuonCreatorTool",m_muonCreatorTool);
  declareProperty("BuildSlowMuon",m_buildSlowMuon=false);
  declareProperty("CreateSAmuons", m_doSA=false);
  declareProperty("MakeClusters",m_makeClusters=true);
}

MuonCreatorAlg::~MuonCreatorAlg(){}

StatusCode MuonCreatorAlg::initialize()
{

  ATH_CHECK(m_muonCreatorTool.retrieve());
  ATH_CHECK(m_muonCollectionName.initialize());
  ATH_CHECK(m_slowMuonCollectionName.initialize(m_buildSlowMuon));
  ATH_CHECK(m_indetCandidateCollectionName.initialize(!m_doSA));
  ATH_CHECK(m_muonCandidateCollectionName.initialize(!m_buildSlowMuon));
  //Can't use a flag in intialize for an array of keys
  if(!m_doSA) ATH_CHECK(m_tagMaps.initialize());
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
  ATH_CHECK(m_clusterContainerName.initialize(m_makeClusters));
  m_clusterContainerLinkName = m_clusterContainerName.key()+"_links";
  ATH_CHECK(m_clusterContainerLinkName.initialize(m_makeClusters));

  return StatusCode::SUCCESS; 
}

StatusCode MuonCreatorAlg::execute()
{

  const InDetCandidateCollection *indetCandidateCollection = 0;
  std::vector<const MuonCombined::InDetCandidateToTagMap*> tagMaps;
  if(!m_doSA){
    SG::ReadHandle<InDetCandidateCollection> indetRH(m_indetCandidateCollectionName);
    if(!indetRH.isValid()){
      ATH_MSG_ERROR("Could not read "<< m_indetCandidateCollectionName);
      return StatusCode::FAILURE;
    }
    else{
      indetCandidateCollection = indetRH.cptr();
    }
    std::vector<SG::ReadHandle<MuonCombined::InDetCandidateToTagMap> > mapHandles=m_tagMaps.makeHandles();
    for(auto& h : mapHandles) tagMaps.push_back(h.cptr());
  }

  // Create the xAOD container and its auxiliary store:
  SG::WriteHandle<xAOD::MuonContainer> wh_muons(m_muonCollectionName);
  ATH_CHECK(wh_muons.record(std::make_unique<xAOD::MuonContainer>(), std::make_unique<xAOD::MuonAuxContainer>()));
  ATH_MSG_DEBUG( "Recorded Muons with key: " << m_muonCollectionName.key() );
  
  MuonCombined::IMuonCreatorTool::OutputData output(*(wh_muons.ptr()));



  // Create and record track particles:
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

  // calo clusters
  SG::WriteHandle<xAOD::CaloClusterContainer> wh_clusters;
  SG::WriteHandle<CaloClusterCellLinkContainer> wh_clusterslink;
  if(m_makeClusters){
    xAOD::CaloClusterContainer *caloclusters = new xAOD::CaloClusterContainer();
    xAOD::CaloClusterAuxContainer *caloclustersaux = new xAOD::CaloClusterAuxContainer();
    caloclusters->setStore(caloclustersaux);
    wh_clusters=SG::WriteHandle<xAOD::CaloClusterContainer>(m_clusterContainerName);
    wh_clusterslink=SG::WriteHandle<CaloClusterCellLinkContainer>(m_clusterContainerLinkName);
    ATH_CHECK(wh_clusters.record(std::unique_ptr<xAOD::CaloClusterContainer>(caloclusters),std::unique_ptr<xAOD::CaloClusterAuxContainer>(caloclustersaux)));
    output.clusterContainer = wh_clusters.ptr();
  }

  const MuonCandidateCollection *muonCandidateCollection =0;

  SG::WriteHandle<xAOD::SlowMuonContainer> wh_slowmuon;
  if (m_buildSlowMuon){
    wh_slowmuon=SG::WriteHandle<xAOD::SlowMuonContainer>(m_slowMuonCollectionName);
    ATH_CHECK(wh_slowmuon.record(std::make_unique<xAOD::SlowMuonContainer>(),std::make_unique<xAOD::SlowMuonAuxContainer>()));
    output.slowMuonContainer=wh_slowmuon.ptr();
  }
  else{
    SG::ReadHandle<MuonCandidateCollection> muonCandidateRH(m_muonCandidateCollectionName);
    if(!muonCandidateRH.isValid()){
      ATH_MSG_ERROR("Could not read "<< m_muonCandidateCollectionName);
      return StatusCode::FAILURE;
    }
    muonCandidateCollection = muonCandidateRH.cptr();
  }
  
  m_muonCreatorTool->create(muonCandidateCollection, indetCandidateCollection, tagMaps, output);

  if(m_makeClusters){
    CaloClusterCellLinkContainer *clusterlinks = new CaloClusterCellLinkContainer();
    auto sg = wh_clusters.storeHandle().get();
    for (xAOD::CaloCluster* cl : *(wh_clusters.ptr())) {
      cl->setLink(clusterlinks, sg);
    }
    ATH_CHECK(wh_clusterslink.record(std::unique_ptr<CaloClusterCellLinkContainer>(clusterlinks)));
  }

  return StatusCode::SUCCESS;
}


StatusCode MuonCreatorAlg::finalize()
{
  return StatusCode::SUCCESS;
}
