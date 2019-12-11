/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TrkTCFex.h"
#include "TrigEFMissingET/METComponent.h"
#include "TrigInterfaces/HLTCheck.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include <memory>
#include <numeric>

namespace {
  constexpr float TRACK_EXTRAP_OUT_OF_ACCEPTANCE = -999;
  std::vector<std::pair<float, float>> bounds{
    {0, 1.5}, {-1.5, 0}, {1.5, 5}, {-5, -1.5}};
} //> end anonymous namespace

namespace HLT { namespace MET {
  TrkTCFex::TrkTCFex(const std::string& name, ISvcLocator* pSvcLocator) :
    FexBase(name, pSvcLocator),
    m_clusModTools(this),
    m_cvfTool("CVFTool/CVFTool", this)
  {
    declareProperty("ClusterModTools", m_clusModTools,
        "The cluster modifier tools (e.g. voronoi and soft killer tools");
    declareProperty("CVFTool", m_cvfTool, "The CVF tool to use");
    declareProperty("CVFThreshold", m_cvfThreshold = 0.1,
        "The CVF threshold to use to determine HS vs PU");
    declareProperty("TrackSoftTermPtCeiling", m_tstPtCeil = 0,
        "The maximum pt for tracks going into the track soft term");
    declareProperty("DoMuonOR", m_doMuonOR=true,
        "Whether or not to do overlap removal between muons and tracks. "
        "Tracks should be provided if and only if this is true.");
  }

  TrkTCFex::~TrkTCFex() {}

  HLT::ErrorCode TrkTCFex::hltInitialize()
  {
    if (!m_clusModTools.retrieve().isSuccess() ) {
      ATH_MSG_ERROR("Failed to retrieve cluster modifier tools!");
      return HLT::ERROR;
    }
    if (!m_cvfTool.retrieve().isSuccess() ) {
      ATH_MSG_ERROR("Failed to retrieve CVF tool");
      return HLT::ERROR;
    }
    std::string trackClusLinkName = m_cvfTool->trackLinkName();
    if (trackClusLinkName.empty() ) {
      ATH_MSG_ERROR("The CVFTool does not provide track-cluster links!");
      return StatusCode::FAILURE;
    }
    m_trackClusLinks = std::make_unique<accTrackLinks_t>(trackClusLinkName);
    // Fill the same components as tc with a track soft term
    return initializeBase({"TCB1", "TCB2", "TCE1", "TCE2", "TrackSoftTerm"});
  }

  HLT::ErrorCode TrkTCFex::hltFinalize()
  {
    return HLT::OK;
  }

  HLT::ErrorCode TrkTCFex::fillMET(
      xAOD::TrigMissingET& met,
      const std::vector<std::vector<HLT::TriggerElement*>>& tes_in)
  {
    const xAOD::CaloClusterContainer* clusters = nullptr;
    const xAOD::TrackParticleContainer* tracks = nullptr;
    const xAOD::VertexContainer* vertices = nullptr;
    const xAOD::MuonContainer* muons = nullptr;
    std::size_t nExpected = m_doMuonOR ? 3 : 2;
    if (tes_in.size() != nExpected || tes_in.at(0).size() != 1 || tes_in.at(1).size() != 1) {
      ATH_MSG_ERROR("Unexpected number of input trigger elements");
      return HLT::NAV_ERROR;
    }
    HLT_CHECK( getFeature(tes_in.at(0).at(0), clusters) );
    HLT_CHECK( getFeature(tes_in.at(1).at(0), tracks) );
    HLT_CHECK( getFeature(tes_in.at(1).at(0), vertices) );
    // TODO. This is the way it works *now*, but it's probably actually wrong...
    // We need every muon, not just the last one
    if (m_doMuonOR && tes_in.at(2).size() != 0) {
      HLT_CHECK( getFeature(tes_in.at(2).back(), muons) );
    }
    if (msgLvl(MSG::DEBUG) ) {
      ATH_MSG_DEBUG("Received " << clusters->size() << " clusters");
      ATH_MSG_DEBUG("Received " << tracks->size() << " tracks");
      ATH_MSG_DEBUG("Received " << vertices->size() << " vertices");
      if (muons) {
        ATH_MSG_DEBUG("Received " << muons->size() << " muons");
      }
      else
        ATH_MSG_DEBUG("Received no muons!");
    }

    // TODO - both trkmht and this algorithm do this - combine them?
    // Get the tracks from our muons
    // If the muon OR is turned off then this is never filled
    std::vector<const xAOD::TrackParticle*> muonTracks;
    if (muons) {
      for (const xAOD::Muon* imu : *muons) {
        auto type = imu->muonType();
        if (type == xAOD::Muon::MuonType::Combined ||
            type == xAOD::Muon::MuonType::SegmentTagged) {
          const xAOD::TrackParticle* idTrk = imu->trackParticle(
              xAOD::Muon::TrackParticleType::InnerDetectorTrackParticle);
          if (!idTrk || imu->pt() < 5*Gaudi::Units::GeV)
            continue;
          muonTracks.push_back(idTrk);
        }
      }
    } //> end if(muons)

    // Find the right vertex
    const xAOD::Vertex* priVtx = nullptr;
    for (const xAOD::Vertex* vtx : *vertices) {
      if (vtx->vertexType() == xAOD::VxType::PriVtx) {
        priVtx = vtx;
        break;
      }
    }

    // The tools we have here *modify* a non-const cluster container. Therefore
    // we need one of those
    xAOD::CaloClusterContainer modClusters;
    xAOD::CaloClusterAuxContainer modClustersAux;
    modClusters.setStore(&modClustersAux);
    modClusters.reserve(clusters->size() );
    for (const xAOD::CaloCluster* iclus : *clusters) {
      xAOD::CaloCluster* clus = new xAOD::CaloCluster();
      modClusters.push_back(clus);
      *clus = *iclus;
    }

    // This does voronoi subtraction and softkiller (if we've given it those
    // tools)
    for (ToolHandle<IJetConstituentModifier>& modTool : m_clusModTools) {
      if (!modTool->process(&modClusters).isSuccess() ) {
        ATH_MSG_ERROR("Failed to modify calo clusters!");
        return HLT::ERROR;
      }
    }
    // Now do CVF
    if (!m_cvfTool->calculateCVF(&modClusters, tracks, priVtx).isSuccess() ) {
      ATH_MSG_ERROR("Failed to calculate CVF!");
      return HLT::ERROR;
    }

    // Get PV tracks that are isolated from the muon tracks
    std::vector<const xAOD::TrackParticle*> isolatedTracks;
    // For this algorithm, PV tracks are taken from the vertex directly
    if (priVtx) {
      for (const trackLink_t& link : priVtx->trackParticleLinks() ) {
        const xAOD::TrackParticle* track = *link;
        // Apparently we skip any that fail the next cuts?
        if (track->pt() < 400 || fabs(track->eta() ) > 2.5)
          continue;
        bool isMuonTrk = false;
        for (const xAOD::TrackParticle* muTrk : muonTracks)
          if (muTrk->p4().DeltaR(track->p4() ) < 0.01) {
            isMuonTrk = true;
            break;
          }
        if (isMuonTrk)
          continue;
        // Skip tracks below 1 GeV
        if (track->pt() < 1*Gaudi::Units::GeV)
          continue;
        if (m_tstPtCeil > 0 && track->pt() > m_tstPtCeil)
          continue;
        isolatedTracks.push_back(track);
      }
    }

    // Now build the sum
    
    std::vector<METComponent> sums(bounds.size() );
    for (const xAOD::CaloCluster* iclus : modClusters) {
      // Skip any that were weighted to 0 or have a CVF not satisfying our
      // selections
      double cvf;
      if (!iclus->retrieveMoment(xAOD::CaloCluster::VERTEX_FRACTION, cvf) ) {
        ATH_MSG_ERROR("Failed to retrieve CVF!");
        return HLT::ERROR;
      }
      if (iclus->e() == 0 || (cvf != -1 && cvf < m_cvfThreshold && iclus->pt() < 3*Gaudi::Units::GeV) )
        continue;
      SignedKinematics kin = SignedKinematics::fromEnergyEtaPhiM(
          iclus->e(xAOD::CaloCluster::CALIBRATED),
          iclus->eta(xAOD::CaloCluster::CALIBRATED),
          iclus->phi(xAOD::CaloCluster::CALIBRATED),
          iclus->calM() );
      // Put it in the right component
      for (std::size_t ii = 0; ii < bounds.size(); ++ii)
        if (bounds.at(ii).first < kin.eta() &&
            kin.eta() <= bounds.at(ii).second)
          sums.at(ii) += kin;

      // Now get the linked tracks and remove them from our isolated tracks
      // vector.
      const vecTrackLink_t& links = (*m_trackClusLinks)(*iclus);
      for (const trackLink_t& link : links) {
        auto itr = std::find(isolatedTracks.begin(), isolatedTracks.end(), *link);
        if (itr != isolatedTracks.end() )
          isolatedTracks.erase(itr);
      }
    }

    METComponent tst;
    for (const xAOD::TrackParticle* track : isolatedTracks)
      tst += track->p4();

    // Save the output
    std::accumulate(sums.begin(), sums.end(), tst).fillMET(met);
    // Save each component
    for (std::size_t ii = 0; ii < sums.size(); ++ii)
      sums.at(ii).fillMETComponent(ii, met);
    // Save the track soft term
    tst.fillMETComponent(4, met);
    return HLT::OK;
  }
} } //> end namespace HLT::MET
