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
  ATH_CHECK(m_segContainerName.initialize());
  m_combinedCollectionName = m_combinedCollectionName.key()+"TrackParticles";
  ATH_CHECK(m_combinedCollectionName.initialize());
  m_extrapolatedCollectionName = m_extrapolatedCollectionName.key()+"TrackParticles";
  ATH_CHECK(m_extrapolatedCollectionName.initialize());
  m_msOnlyExtrapolatedCollectionName = m_msOnlyExtrapolatedCollectionName.key()+"TrackParticles";
  ATH_CHECK(m_msOnlyExtrapolatedCollectionName.initialize());
  ATH_CHECK(m_clusterContainerName.initialize(m_makeClusters));
  m_clusterContainerLinkName = m_clusterContainerName.key()+"_links";
  ATH_CHECK(m_clusterContainerLinkName.initialize(m_makeClusters));

  if ( not m_monTool.name().empty() ) {
    ATH_CHECK( m_monTool.retrieve() );
  }

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
  MuonCombined::IMuonCreatorTool::OutputData output(*(wh_muons.ptr()));

  // Create and record track particles:
  //combined tracks
  SG::WriteHandle<xAOD::TrackParticleContainer> wh_combtp(m_combinedCollectionName);
  ATH_CHECK(wh_combtp.record(std::make_unique<xAOD::TrackParticleContainer>(), std::make_unique<xAOD::TrackParticleAuxContainer>()));
  output.combinedTrackParticleContainer = wh_combtp.ptr();

  //extrapolated tracks
  SG::WriteHandle<xAOD::TrackParticleContainer> wh_extrtp(m_extrapolatedCollectionName);
  ATH_CHECK(wh_extrtp.record(std::make_unique<xAOD::TrackParticleContainer>(), std::make_unique<xAOD::TrackParticleAuxContainer>()));
  output.extrapolatedTrackParticleContainer = wh_extrtp.ptr();

  //msonly tracks
  SG::WriteHandle<xAOD::TrackParticleContainer> wh_msextrtp(m_msOnlyExtrapolatedCollectionName);
  ATH_CHECK(wh_msextrtp.record(std::make_unique<xAOD::TrackParticleContainer>(), std::make_unique<xAOD::TrackParticleAuxContainer>()));
  output.msOnlyExtrapolatedTrackParticleContainer = wh_msextrtp.ptr();

  //segments
  SG::WriteHandle<xAOD::MuonSegmentContainer> wh_segment(m_segContainerName);
  ATH_CHECK(wh_segment.record(std::make_unique<xAOD::MuonSegmentContainer>(),std::make_unique<xAOD::MuonSegmentAuxContainer>()));
  output.xaodSegmentContainer=wh_segment.ptr();

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
  //------------ Last modified: 17/09/2019                                                                   ------------//
  //---------------------------------------------------------------------------------------------------------------------//

  // Only run monitoring for online algorithms
  if ( not m_monTool.name().empty() ) {
    // Variables to initialize and keep for monitoring
    std::vector<int> ini_mun(0);
    std::vector<double> ini_mupt(0);
    std::vector<double> ini_mueta(0);
    std::vector<double> ini_muphi(0);
    std::vector<int> ini_satrksn(0);
    std::vector<double> ini_satrkspt(0);
    std::vector<double> ini_satrkseta(0);
    std::vector<double> ini_satrksphi(0);
    std::vector<int> ini_cbtrksn(0);
    std::vector<double> ini_cbtrkspt(0);
    std::vector<double> ini_cbtrkseta(0);
    std::vector<double> ini_cbtrksphi(0);

    // Monitoring histograms
    auto muon_n       = Monitored::Collection("muon_n", ini_mun);
    auto muon_pt      = Monitored::Collection("muon_pt", ini_mupt);
    auto muon_eta     = Monitored::Collection("muon_eta", ini_mueta);
    auto muon_phi     = Monitored::Collection("muon_phi", ini_muphi);
    auto satrks_n     = Monitored::Collection("satrks_n", ini_satrksn);
    auto satrks_pt    = Monitored::Collection("satrks_pt", ini_satrkspt);
    auto satrks_eta   = Monitored::Collection("satrks_eta", ini_satrkseta);
    auto satrks_phi   = Monitored::Collection("satrks_phi", ini_satrksphi);
    auto cbtrks_n     = Monitored::Collection("cbtrks_n", ini_cbtrksn);
    auto cbtrks_pt    = Monitored::Collection("cbtrks_pt", ini_cbtrkspt);
    auto cbtrks_eta   = Monitored::Collection("cbtrks_eta", ini_cbtrkseta);
    auto cbtrks_phi   = Monitored::Collection("cbtrks_phi", ini_cbtrksphi);
    
    auto monitorIt = Monitored::Group(m_monTool, muon_n, muon_pt, muon_eta, muon_phi, satrks_n, satrks_pt, satrks_eta, satrks_phi, cbtrks_n, cbtrks_pt, cbtrks_eta, cbtrks_phi);
    
    // Muon
    ini_mun.push_back(wh_muons->size());
    for (auto const& muon : *(wh_muons.ptr())) {
      ini_mupt.push_back(muon->pt()/1000.0); // converted to GeV
      ini_mueta.push_back(muon->eta());
      ini_muphi.push_back(muon->phi());
    }

    // Extrapolated tracks
    for (auto const& satrk : *(wh_extrtp.ptr())) {      
      ini_satrkspt.push_back(satrk->pt()/1000.0); // converted to GeV
      ini_satrkseta.push_back(satrk->eta());
      ini_satrksphi.push_back(satrk->phi());
    }

    // Combined tracks
    for (auto const& cbtrk : *(wh_combtp.ptr())) {
	    ini_cbtrkspt.push_back(cbtrk->pt()/1000.0); // converted to GeV
	    ini_cbtrkseta.push_back(cbtrk->eta());
	    ini_cbtrksphi.push_back(cbtrk->phi());
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode MuonCreatorAlg::finalize()
{
  return StatusCode::SUCCESS;
}
