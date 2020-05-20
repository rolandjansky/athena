/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TrkMHTFex.h"
#include "TrigEFMissingET/METComponent.h"
#include "TrigInterfaces/HLTCheck.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "PathResolver/PathResolver.h"
#include <TFile.h>
#include <numeric>

namespace {
  std::ostream& operator<<(std::ostream& os, const xAOD::IParticle& ipart) {
    os << "(pt, eta, phi, m) = (" 
      << ipart.pt() << ", "
      << ipart.eta() << ", "
      << ipart.phi() << ", "
      << ipart.m() << ")";
    return os;
  }
  std::ostream& operator<<(std::ostream& os, const xAOD::TrackParticle& trk) {
    os << "(pt, eta, phi, vertex, z0, vz) = ("
      << trk.pt() << ", "
      << trk.eta() << ", "
      << trk.phi() << ", "
      << trk.vertex() << ", "
      << trk.z0() << ", "
      << trk.vz() << ")";
    return os;
  }
  std::ostream& operator<<(std::ostream& os, const xAOD::Vertex& vtx) {
    os << "(x, y, z, ntracks) = ("
      << vtx.x() << ", "
      << vtx.y() << ", "
      << vtx.z() << ", "
      << vtx.nTrackParticles() << ")";
    return os;
  }
} //> end anonymous namespace

namespace HLT { namespace MET {
  TrkMHTFex::TrkMHTFex(const std::string& name, ISvcLocator* pSvcLocator) :
    FexBase(name, pSvcLocator),
    m_trackSelTool("InDet::InDetTrackSelectionTool/TrackSelectionTool", this)
  {
    declareProperty("EtaCut", m_barrelEndcapEta=2.2,
        "Where to make the split between central and forward jets.");
    declareProperty("CentralPtCut", m_centralPtCut = 0.,
        "pT Cut [MeV] to use for central jets");
    declareProperty("ForwardPtCut", m_forwardPtCut = 0.,
        "pT Cut [MeV] to use for forward jets");
    declareProperty("TrackPtCut", m_trackPtCut = 0.0,
        "pT Cut [MeV] to use for tracks going into the track soft term");
    declareProperty("JVTCut", m_jvtCut = 0.9, "JVT cut to apply to jets");
    declareProperty("TrackSelectionTool", m_trackSelTool,
        "The tool to use to select tracks");
    declareProperty("JVTFileName", m_jvtFile = "JVTlikelihood_20140805.root",
        "The ROOT file containing the JVT LUT");
    declareProperty("JVTHistoName",
        m_jvtHistoName = "JVTRootCore_kNN100trim_pt20to50_Likelihood",
        "The JVT LUT");
    declareProperty("MinJetJVTPt", m_minJetJVTPt = 20 * Gaudi::Units::GeV,
        "The minimum jet pT to apply JVT to");
    declareProperty("MaxJetJVTPt", m_maxJetJVTPt = 50 * Gaudi::Units::GeV,
        "The maximum jet pT to apply JVT to");
    declareProperty("TrackSoftTermPtCeiling", m_tstPtCeil = 0,
        "The maximum pt for tracks going into the track soft term");
    declareProperty("DoMuonOR", m_doMuonOR=false,
        "Whether or not to do overlap removal between muons and tracks. "
        "Tracks should be provided if and only if this is true.");
  }

  HLT::ErrorCode TrkMHTFex::hltInitialize()
  {
    if (!m_trackSelTool.retrieve() ) {
      ATH_MSG_ERROR("Failed to retrieve track selection tool!");
      return HLT::ERROR;
    }
    // Try to get the JVT histogram
    std::string resolvedName = PathResolverFindCalibFile(m_jvtFile);
    if (resolvedName.empty() ) {
      ATH_MSG_ERROR("Failed to find file " << m_jvtFile);
      return HLT::ERROR;
    }
    TFile fp(resolvedName.c_str(), "READ");
    if (fp.IsZombie() ) {
      ATH_MSG_ERROR("Failed to open file " << resolvedName);
      return HLT::ERROR;
    }
    TH2F* histo = nullptr;
    fp.GetObject(m_jvtHistoName.c_str(), histo);
    if (!histo) {
      ATH_MSG_ERROR("Failed to retrieve " << m_jvtHistoName
          << " from file " << resolvedName);
      return HLT::ERROR;
    }
    m_jvtLUT = *histo;
    // just in case
    m_jvtLUT.SetDirectory(0);

     std::vector<std::string> componentNames;
    // Set the bounds of the different regions we consider
    // Note that later parts of the code will assume that these are exclusive
    // regions.
    // JETB1 == positive side of the barrel
    componentNames.push_back("JETB1");
    m_bounds.push_back(std::make_pair(0, m_barrelEndcapEta) );
    // JETB2 == negative side of the barrel
    componentNames.push_back("JETB2");
    m_bounds.push_back(std::make_pair(-m_barrelEndcapEta, 0) );
    // JETE1 == positive side of the endcap
    componentNames.push_back("JETE1");
    m_bounds.push_back(std::make_pair(m_barrelEndcapEta, 5) );
    // JETE2 == negative side of the endcap
    componentNames.push_back("JETE2");
    m_bounds.push_back(std::make_pair(-5, m_barrelEndcapEta) );
    componentNames.push_back("TrackSoftTerm");
    return initializeBase(componentNames);
 }

  HLT::ErrorCode TrkMHTFex::hltFinalize()
  {
    return HLT::OK;
  }

  HLT::ErrorCode TrkMHTFex::fillMET(
      xAOD::TrigMissingET& met,
      const std::vector<std::vector<HLT::TriggerElement*>>& tes_in)
  {
    // Get the input containers
    const xAOD::JetContainer* jets = nullptr;
    const xAOD::TrackParticleContainer* tracks = nullptr;
    const xAOD::VertexContainer* vertices = nullptr;
    const xAOD::MuonContainer* muons = nullptr;
    std::size_t nExpected = m_doMuonOR ? 3 : 2;
    if (tes_in.size() != nExpected || tes_in.at(0).size() != 1 || tes_in.at(1).size() != 1) {
      ATH_MSG_ERROR("Unexpected number of input trigger elements");
      return HLT::NAV_ERROR;
    }
    HLT_CHECK( getFeature(tes_in.at(0).at(0), jets) );
    HLT_CHECK( getFeature(tes_in.at(1).at(0), tracks) );
    HLT_CHECK( getFeature(tes_in.at(1).at(0), vertices) );
    // TODO. This is the way it works *now*, but it's probably actually wrong...
    // We need every muon, not just the last one
    if (m_doMuonOR && tes_in.at(2).size() != 0) {
      HLT_CHECK( getFeature(tes_in.at(2).back(), muons) );
    }
    if (msgLvl(MSG::DEBUG) ) {
      ATH_MSG_DEBUG("Pointers: " << jets << ", " << tracks << ", " << vertices);
      ATH_MSG_DEBUG("Received " << jets->size() << " jets");
      for (const xAOD::Jet* ijet : *jets) 
        ATH_MSG_DEBUG(*ijet);
      ATH_MSG_DEBUG("Received " << tracks->size() << " tracks");
      for (const xAOD::TrackParticle* itrk : *tracks)
        ATH_MSG_DEBUG(*itrk);
      ATH_MSG_DEBUG("Received " << vertices->size() << " vertices");
      for (const xAOD::Vertex* ivtx : *vertices)
        ATH_MSG_DEBUG(*ivtx);
      if (muons) {
        ATH_MSG_DEBUG("Received " << muons->size() << " muons");
        for (const xAOD::Muon* imu : *muons)
          ATH_MSG_DEBUG(*imu);
      }
      else
        ATH_MSG_DEBUG("Received no muons!");
    }

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

    std::vector<METComponent> mhtSums(m_bounds.size() );
    METComponent tst;

    if (priVtx) {

      // Split tracks into pileup and primary vertex
      std::vector<const xAOD::TrackParticle*> pileupTracks;
      std::vector<const xAOD::TrackParticle*> pvTracks;
      std::size_t nPUTracks = 0;

      for (const xAOD::TrackParticle* trk : *tracks) {
        // Skip any track not accepted by the tool
        if (!m_trackSelTool->accept(*trk, priVtx) )
          continue;
        // Work out if the track was from the primary vertex
        bool isFromPV = (
            trk->vertex()
            ? trk->vertex() == priVtx
            : fabs((trk->z0() + trk->vz() - priVtx->z()) * sin(trk->theta() ) ) < 3.0);
        if (isFromPV) {
          pvTracks.push_back(trk);
        }
        else {
          pileupTracks.push_back(trk);
          if (trk->pt() < 30 * Gaudi::Units::GeV)
            ++nPUTracks;
        }
      }
      // Make sure we don't divide by 0 later
      if (nPUTracks == 0)
        ++nPUTracks;

      // Keep a list of good jets
      std::vector<const xAOD::Jet*> goodJets;
      // TODO - this is to keep with the current tool, really this should be the
      // eta threshold use to split the components (i.e. we should move *that*
      // to 2.4 - and it probably shouldn't be configurable any more)
      for (const xAOD::Jet* ijet : *jets) {
        if (ijet->pt() < (fabs(ijet->eta()) > 2.4
              ? m_forwardPtCut
              : m_centralPtCut) )
          continue;
        // This next part is to keep with the old logic. We probably want to drop
        // it (anything with pT too low for the JVT cut shouldn't be included).
        if (fabs(ijet->eta()) > 2.4 ||
            ijet->pt() < m_minJetJVTPt ||
            ijet->pt() > m_maxJetJVTPt) {
          goodJets.push_back(ijet);
          continue;
        }

        float pvSum = 0;
        float pileupSum = 0;
        for (const xAOD::TrackParticle* trk : pvTracks)
          if (trk->p4().DeltaR(ijet->p4() ) < 0.4 && trk->pt() > 500)
            pvSum += trk->pt();
        for (const xAOD::TrackParticle* trk : pileupTracks)
          if (trk->p4().DeltaR(ijet->p4() ) < 0.4 && trk->pt() > 500)
            pileupSum += trk->pt();

        if (pvSum + pileupSum > 0) {
          float Rpt = pvSum / ijet->pt();
          float corrJVF = pvSum / (pvSum + 100*pileupSum/nPUTracks);
          double JVT = m_jvtLUT.Interpolate(corrJVF, std::min(Rpt, 1.0f) );
          if (JVT > m_jvtCut)
            goodJets.push_back(ijet);
        }
      }

      // Now get the list of isolated tracks (not part of any good jet or muon)
      std::vector<const xAOD::TrackParticle*> isolatedTracks;
      for (const xAOD::TrackParticle* trk : pvTracks) {
        if (trk->pt() < m_trackPtCut)
          continue;
        if (m_tstPtCeil > 0 && trk->pt() > m_tstPtCeil)
          continue;
        if (fabs(trk->eta() ) > 2.4)
          continue;
        bool isIsolated = true;
        // First remove any that are within 0.4 of a good jet.
        // TODO - maybe it's possible to keep this decision somewhere from before?
        for (const xAOD::Jet* ijet : goodJets)
          if (ijet->p4().DeltaR(trk->p4() ) < 0.4) {
            isIsolated = false;
            break;
          }
        if (!isIsolated)
          continue;
        // Now the same for muons
        for (const xAOD::TrackParticle* muTrk : muonTracks)
          if (muTrk->p4().DeltaR(trk->p4() ) < 0.1) {
            isIsolated = false;
            break;
          }
        if (!isIsolated)
          continue;
        isolatedTracks.push_back(trk);
      }

      // Now we can do the actual sum
      for (const xAOD::Jet* ijet : goodJets) {
        float eta = ijet->eta();
        for (std::size_t ii = 0; ii < m_bounds.size(); ++ii)
          if (m_bounds.at(ii).first < eta && eta <= m_bounds.at(ii).second) {
            mhtSums.at(ii) += ijet->p4();
            break;
          }
      }
      for (const xAOD::TrackParticle* trk : isolatedTracks)
        tst += trk->p4();
    } //> end if (priVtx) => if there was no primary vertex, output 0

    // Save the sum over all components
    std::accumulate(mhtSums.begin(), mhtSums.end(), tst).fillMET(met);
    // Save each component
    for (std::size_t ii = 0; ii < mhtSums.size(); ++ii)
      mhtSums.at(ii).fillMETComponent(ii, met);
    // Save the TST
    tst.fillMETComponent(4, met);

    ATH_MSG_DEBUG("TST: " << tst);
    ATH_MSG_DEBUG("Jets: " << std::accumulate(mhtSums.begin(), mhtSums.end(), METComponent{}) );

    return HLT::OK;
  }

} } //> end namespace HLT::MET
