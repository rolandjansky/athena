/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonCombinedTimingTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MuonCombinedTimingTool.h"

#include "TrkSpaceTimePoint/SpaceTimePoint.h"


//================ Constructor =================================================

Rec::MuonCombinedTimingTool::MuonCombinedTimingTool(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t, n, p)
{
    declareInterface<IMuonCombinedTimingTool>(this);
}

//================ Destructor =================================================

Rec::MuonCombinedTimingTool::~MuonCombinedTimingTool() {}


//================ Initialisation =================================================

StatusCode
Rec::MuonCombinedTimingTool::initialize()
{

    StatusCode sc = AlgTool::initialize();
    if (sc.isFailure()) return sc;

    if (!m_trtTimingTool.empty()) {
        ATH_CHECK(m_trtTimingTool.retrieve());
        ATH_MSG_DEBUG("Retrieved " << m_trtTimingTool.name());
    }
    if (!m_caloTimingTool.empty()) {
        ATH_CHECK(m_caloTimingTool.retrieve());
        ATH_MSG_DEBUG("Retrieved " << m_caloTimingTool.name());
    }
    if (!m_mdtSegmentTimingTool.empty()) {
        ATH_CHECK(m_mdtSegmentTimingTool.retrieve());
        ATH_MSG_DEBUG("Retrieved " << m_mdtSegmentTimingTool.name());
    }
    if (!m_rpcTimingTool.empty()) {
        ATH_CHECK(m_rpcTimingTool.retrieve());
        ATH_MSG_DEBUG("Retrieved " << m_rpcTimingTool.name());
    }

    /*
    if (!m_cscTimingTool.empty()) {
      if (m_cscTimingTool.retrieve().isFailure()) {
        ATH_MSG_ERROR ("Could not retrieve "<<m_cscTimingTool.name());
        return StatusCode::FAILURE;
      } else ATH_MSG_DEBUG ("Retrieved " << m_cscTimingTool.name());
      } */

    ATH_MSG_DEBUG("initialize() successful in " << name());
    return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode
Rec::MuonCombinedTimingTool::finalize()
{
    return StatusCode::SUCCESS;
}

//============================================================================================
std::vector<Trk::SpaceTimePoint*>
Rec::MuonCombinedTimingTool::timeMeasurements(const xAOD::Muon&) const
{
    std::vector<Trk::SpaceTimePoint*> spaceTimePoints;  // empty vector
    ATH_MSG_WARNING(
        "This tool is not migrated to xAOD yet, please volunteer to update it if you need the information filled");
    //   // return empty vector in case no segment container was associated to the muon
    //   if (!getSegmentContainer(muon)) return spaceTimePoints;

    //   // ***
    //   // *** first choice: primary author combined muon or MuGirl refitted
    //   // ***
    //   if (muon.hasCombinedMuonTrackParticle() ) {
    // //  || muon.hasInnerExtrapolatedTrackParticle() ) // analysis will be based on MuGirl segments

    //     const Rec::TrackParticle* muonParticle = muon.hasCombinedMuonTrackParticle() ?
    //     muon.combinedMuonTrackParticle() : muon.innerExtrapolatedTrackParticle();

    //     const Trk::Track* combinedTrack = muonParticle ? muonParticle->originalTrack() : NULL;

    //     if (combinedTrack==NULL) return spaceTimePoints;

    //     const Trk::Track* inDetTrack = muon.inDetTrkTrack();
    //     if (!m_trtTimingTool.empty() && inDetTrack!= NULL) {
    //       // in case a refit is attempted, better give only the ID track to the tool.
    //       std::vector<Trk::SpaceTimePoint*> v1
    // 	= m_trtTimingTool->timeMeasurements(*inDetTrack);
    //       if (!v1.empty()) spaceTimePoints.insert( spaceTimePoints.end(), v1.begin(), v1.end());
    //     }

    //     if (!m_caloTimingTool.empty()) {
    //       std::vector<Trk::SpaceTimePoint*> v2 = m_caloTimingTool->timeMeasurements(*combinedTrack);
    //       if (!v2.empty()) spaceTimePoints.insert( spaceTimePoints.end(), v2.begin(), v2.end());
    //     }

    //     if (!m_mdtSegmentTimingTool.empty()) {
    //       const Trk::SegmentCollection* segments = getSegmentContainer(muon);
    //       std::vector<Trk::SpaceTimePoint*> v3 = m_mdtSegmentTimingTool->timeMeasurements(*combinedTrack, segments);
    //       if (!v3.empty()) spaceTimePoints.insert( spaceTimePoints.end(), v3.begin(), v3.end());
    //       // if at some point consider passing tracks, in case t0 determined on the fly
    //       // std::vector<Trk::SpaceTimePoint*> v3 = m_mdtSegmentTimingTool->timeMeasurements(*combinedTrack);
    //       // if (!v3.empty()) spaceTimePoints.insert( spaceTimePoints.end(), v3.begin(), v3.end());
    //     }
    //     if (!m_rpcTimingTool.empty()) {
    //       std::vector<Trk::SpaceTimePoint*> v4 = m_rpcTimingTool->timeMeasurements(*combinedTrack);
    //       if (!v4.empty()) spaceTimePoints.insert( spaceTimePoints.end(), v4.begin(), v4.end());
    //     }
    //     return spaceTimePoints;
    //   }

    //   // ***
    //   // *** second choice: primary author segment-tagger
    //   // ***
    //   if ( (muon.hasInDetTrackParticle() || muon.hasInnerExtrapolatedTrackParticle()) && !muon.isCaloMuonId()) {
    //     const Rec::TrackParticle* idParticle = muon.inDetTrackParticle();
    //     const Trk::Track* idTrack = idParticle ? idParticle->originalTrack() : NULL;
    //     if (idTrack==NULL) return spaceTimePoints;
    //     if (!m_trtTimingTool.empty()) {
    //       std::vector<Trk::SpaceTimePoint*> v1 = m_trtTimingTool->timeMeasurements(*idTrack);
    //       if (!v1.empty()) spaceTimePoints.insert( spaceTimePoints.end(), v1.begin(), v1.end());
    //     }
    //     if (!m_caloTimingTool.empty()) {
    //       std::vector<Trk::SpaceTimePoint*> v2 = m_caloTimingTool->timeMeasurements(*idTrack);
    //       if (!v2.empty()) spaceTimePoints.insert( spaceTimePoints.end(), v2.begin(), v2.end());
    //     }
    //     if (!m_mdtSegmentTimingTool.empty() || !m_rpcTimingTool.empty()) {
    //       std::vector<const Trk::Segment*> segments;
    //       unsigned int nSeg = muon.numberOfSegments();
    //       segments.reserve(nSeg);
    //       for( unsigned int si=0;si<nSeg;++si ) {
    //         const Trk::Segment* seg = muon.muonSegment(si);
    //         if( seg ) segments.push_back(seg);
    //       }
    //       if (!m_mdtSegmentTimingTool.empty()) {
    // 	std::vector<Trk::SpaceTimePoint*> v3 = m_mdtSegmentTimingTool->timeMeasurements(segments);
    //         if (!v3.empty()) spaceTimePoints.insert( spaceTimePoints.end(), v3.begin(), v3.end());
    //       }
    //       if (!m_rpcTimingTool.empty()) {
    //         std::vector<Trk::SpaceTimePoint*> v4 = m_rpcTimingTool->timeMeasurements(segments);
    //         if (!v4.empty()) spaceTimePoints.insert( spaceTimePoints.end(), v4.begin(), v4.end());
    //       }
    //     }
    //     return spaceTimePoints;
    //   }

    //   // ***
    //   // *** third choice: standalone muon
    //   // ***
    //   if (muon.hasMuonExtrapolatedTrackParticle()){
    //     const Rec::TrackParticle* muonParticle = muon.muonExtrapolatedTrackParticle();
    //     const Trk::Track* msTrack = muonParticle ? muonParticle->originalTrack() : NULL;
    //     if (msTrack==NULL) {
    //        ATH_MSG_WARNING ("No muon track or segments associated to SA muon !?");
    //        return spaceTimePoints;
    //      }
    //     if (!m_caloTimingTool.empty()) {
    //       std::vector<Trk::SpaceTimePoint*> v2 = m_caloTimingTool->timeMeasurements(*msTrack);
    //       if (!v2.empty()) spaceTimePoints.insert( spaceTimePoints.end(), v2.begin(), v2.end());
    //     }
    //     if (!m_mdtSegmentTimingTool.empty()) {
    //       const Trk::SegmentCollection* segments = getSegmentContainer(muon);
    //       std::vector<Trk::SpaceTimePoint*> v3 = m_mdtSegmentTimingTool->timeMeasurements(*msTrack, segments);
    //       if (!v3.empty()) spaceTimePoints.insert( spaceTimePoints.end(), v3.begin(), v3.end());
    //     }
    //     if (!m_rpcTimingTool.empty()) {
    //       std::vector<Trk::SpaceTimePoint*> v4 = m_rpcTimingTool->timeMeasurements(*msTrack);
    //       if (!v4.empty()) spaceTimePoints.insert( spaceTimePoints.end(), v4.begin(), v4.end());
    //     }
    //     return spaceTimePoints;
    //   }

    //   // ***
    //   // *** fourth choice: calo tagged muon
    //   // ***
    //   if (muon.hasInDetTrackParticle() && muon.isCaloMuonId()){
    //     const Rec::TrackParticle* idParticle = muon.inDetTrackParticle();
    //     const Trk::Track* idTrack = idParticle ? idParticle->originalTrack() : NULL;
    //     if (idTrack==NULL) return spaceTimePoints;
    //     if (!m_trtTimingTool.empty()) {
    //       std::vector<Trk::SpaceTimePoint*> v1 = m_trtTimingTool->timeMeasurements(*idTrack);
    //       if (!v1.empty()) spaceTimePoints.insert( spaceTimePoints.end(), v1.begin(), v1.end());
    //     }
    //     if (!m_caloTimingTool.empty()) {
    //       std::vector<Trk::SpaceTimePoint*> v2 = m_caloTimingTool->timeMeasurements(*idTrack);
    //       if (!v2.empty()) spaceTimePoints.insert( spaceTimePoints.end(), v2.begin(), v2.end());
    //     }
    //     return spaceTimePoints;
    //   }

    return spaceTimePoints;
}


// Helper function to figure out which is the appropriate segment container to retrieve
// Based on the muon's author
const Trk::SegmentCollection*
Rec::MuonCombinedTimingTool::getSegmentContainer(const xAOD::Muon&) const
{
    //   std::string containerName = "";

    //   MuonParameters::Author auth = muon.author();

    //   // FIXME At the moment, space-time points are constructed only for muid.
    //   // TODO In the future, 3rd muon chain has to be treated as well.
    //   //First check if muon is Muonboy/STACO
    // //  if(auth == MuonParameters::MuonboySP || auth == MuonParameters::Muonboy ||
    // //     auth == MuonParameters::STACO) {// MuonboySP or Muonboy or STACO
    // //    containerName = "ConvertedMBoySegments";
    // //  } else
    //   if (auth == MuonParameters::MOORE || auth == MuonParameters::MuidSA ||
    // 	     auth == MuonParameters::MuidCo) { //MOORE or MuidSA or MuidCo
    //     containerName = "MooreSegments";
    //   }

    //   if( evtStore()->contains<Trk::SegmentCollection>(containerName) ){
    //     const Trk::SegmentCollection* segs = NULL;

    //     if( evtStore()->retrieve(segs, containerName).isFailure() ){
    //       ATH_MSG_DEBUG("Could not find segment container");
    //     }else{
    //       ATH_MSG_DEBUG("Successfully retrieved segment container");

    //       return segs;
    //     }
    //   }

    return NULL;
}
