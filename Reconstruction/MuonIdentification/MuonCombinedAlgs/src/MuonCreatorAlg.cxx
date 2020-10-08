/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCreatorAlg.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/MuonSegmentAuxContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrkSegment/SegmentCollection.h"
#include "MuonSegment/MuonSegment.h"
#include "xAODMuon/SlowMuonContainer.h"
#include "xAODMuon/SlowMuonAuxContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

#include <vector>

MuonCreatorAlg::MuonCreatorAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator) {
}

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
  m_msOnlyExtrapolatedCollectionName = m_msOnlyExtrapolatedCollectionName.key()+"TrackParticles";
  m_msOnlyExtrapolatedTrkCollectionName = m_msOnlyExtrapolatedCollectionName.key()+"Tracks";
  ATH_CHECK(m_msOnlyExtrapolatedCollectionName.initialize());
  ATH_CHECK(m_msOnlyExtrapolatedTrkCollectionName.initialize());
  ATH_CHECK(m_clusterContainerName.initialize(m_makeClusters));
  m_clusterContainerLinkName = m_clusterContainerName.key()+"_links";
  ATH_CHECK(m_clusterContainerLinkName.initialize(m_makeClusters));

  if ( not m_monTool.name().empty() ) {
    ATH_CHECK( m_monTool.retrieve() );
  }
  ATH_MSG_DEBUG("MuonCreatorAlg has been setup  successfully");
  ATH_MSG_DEBUG("    *** SegmentTrackContainer:            "<<m_segTrkContainerName);
  ATH_MSG_DEBUG("    *** SegmentContainer:                 "<<m_segContainerName);
  ATH_MSG_DEBUG("    *** CombinedTrackContainer:           "<<m_combinedTrkCollectionName);
  ATH_MSG_DEBUG("    *** xAOD::CombinedTrackContainer:     "<<m_combinedCollectionName);
 
  ATH_MSG_DEBUG("    *** xAOD::ExtrpolatedTrackContainer:  "<<m_extrapolatedCollectionName);
  ATH_MSG_DEBUG("    *** ExtrpolatedTrackContainer:        "<<m_extrapolatedTrkCollectionName);  
  ATH_MSG_DEBUG("    *** xAOD::MSOnlyTrackContainer:       "<<m_msOnlyExtrapolatedCollectionName);
  ATH_MSG_DEBUG("    *** ExtrpolatedTackContainer:         "<<m_msOnlyExtrapolatedTrkCollectionName);
  

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
  ATH_MSG_DEBUG( "Recorded Muons with key: " << m_muonCollectionName.key());
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

  //---------------------------------------------------------------------------------------------------------------------//
  //------------                Monitoring of the reconstructed muons inside the trigger algs                ------------//
  //------------ Author:        Laurynas Mince                                                               ------------//
  //------------ Created:       26.07.2019                                                                   ------------//
  //---------------------------------------------------------------------------------------------------------------------//

  // Only run monitoring for online algorithms
  if ( not m_monTool.name().empty() ) {
    // Monitoring histograms and variables
    auto muon_n     = Monitored::Scalar<int>("muon_n", wh_muons->size());
    auto muon_pt    = Monitored::Collection("muon_pt",    *(wh_muons.ptr()),  [](auto const& mu) {return mu->pt()/1000.0;}); // converted to GeV
    auto muon_eta   = Monitored::Collection("muon_eta",   *(wh_muons.ptr()),  &xAOD::Muon_v1::eta);
    auto muon_phi   = Monitored::Collection("muon_phi",   *(wh_muons.ptr()),  &xAOD::Muon_v1::phi);

    auto satrks_n     = Monitored::Scalar<int>("satrks_n", wh_extrtp->size());
    auto satrks_pt  = Monitored::Collection("satrks_pt",  *(wh_extrtp.ptr()), [](auto const& satrk) {return satrk->pt()/1000.0;}); // converted to GeV
    auto satrks_eta = Monitored::Collection("satrks_eta", *(wh_extrtp.ptr()), &xAOD::TrackParticle_v1::eta);
    auto satrks_phi = Monitored::Collection("satrks_phi", *(wh_extrtp.ptr()), &xAOD::TrackParticle_v1::phi);

    auto cbtrks_n   = Monitored::Scalar<int>("cbtrks_n", wh_combtp->size());    
    auto cbtrks_pt  = Monitored::Collection("cbtrks_pt",  *(wh_combtp.ptr()), [](auto const& cbtrk) {return cbtrk->pt()/1000.0;}); // converted to GeV
    auto cbtrks_eta = Monitored::Collection("cbtrks_eta", *(wh_combtp.ptr()), &xAOD::TrackParticle_v1::eta);
    auto cbtrks_phi = Monitored::Collection("cbtrks_phi", *(wh_combtp.ptr()), &xAOD::TrackParticle_v1::phi);

    if (!m_doSA) {
      auto idtrks_n   = Monitored::Scalar<int>("idtrks_n", indetCandidateCollection->size());
      auto idtrks_pt  = Monitored::Collection("idtrks_pt", *indetCandidateCollection, [](auto const& idtrk) {return idtrk->indetTrackParticle().pt()/1000.0;});
      auto idtrks_eta = Monitored::Collection("idtrks_eta", *indetCandidateCollection, [](auto const& idtrk) {return idtrk->indetTrackParticle().eta();});
      auto idtrks_phi = Monitored::Collection("idtrks_phi", *indetCandidateCollection, [](auto const& idtrk) {return idtrk->indetTrackParticle().phi();});
      auto monitorIt = Monitored::Group(m_monTool, muon_n, muon_pt, muon_eta, muon_phi, satrks_n, satrks_pt, satrks_eta, satrks_phi, cbtrks_n, cbtrks_pt,
					cbtrks_eta, cbtrks_phi, idtrks_n, idtrks_pt, idtrks_eta, idtrks_phi);
    }
    else
      auto monitorIt = Monitored::Group(m_monTool, muon_n, muon_pt, muon_eta, muon_phi, satrks_n, satrks_pt, satrks_eta, satrks_phi, cbtrks_n, cbtrks_pt,
                                      cbtrks_eta, cbtrks_phi);
  }

  return StatusCode::SUCCESS;
}
