/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "JetMomentTools/JetDRTrackAssocTool.h"
#include <sstream>
#include "AsgDataHandles/WriteDecorHandle.h"
#include "FourMomUtils/xAODP4Helpers.h"

JetDRTrackAssocTool::JetDRTrackAssocTool(const std::string& name)
    : asg::AsgTool(name)
{
}


//**********************************************************************

StatusCode JetDRTrackAssocTool::initialize() {

  ATH_MSG_INFO("Initializing JetDRTrackAssocTool " << name());

  if(m_jetContainerName.empty()){
    ATH_MSG_ERROR("JetDRTrackAssocTool needs to have its input container name configured!");
    return StatusCode::FAILURE;
  }

  m_decorDRTracks = m_jetContainerName + ".DRTrack";

  ATH_CHECK(m_trackContainer_key.initialize());
  ATH_CHECK(m_decorDRTracks.initialize());

  return StatusCode::SUCCESS;
}

//**********************************************************************


StatusCode JetDRTrackAssocTool::decorate(const xAOD::JetContainer& jets) const {

  // Get input vertex collection
  auto handle_trk = SG::makeHandle (m_trackContainer_key);
  if (!handle_trk.isValid()){
    ATH_MSG_ERROR("Could not retrieve the TrackParticleContainer: "
                  << m_trackContainer_key.key());
    return StatusCode::FAILURE;
  }

  // Generate map of track to closest jet
  // Later we invert this, to ensure that each track only
  // gets associated to one jet
  std::map<const xAOD::TrackParticle*, const xAOD::Jet*> closest_jet_to_track;
  ATH_CHECK( fillTrackToJetMap(closest_jet_to_track,*handle_trk,jets,m_dRcut) );

  // Prepare a vector of track links for each jet, starts empty
  std::vector<std::vector<ElementLink<xAOD::IParticleContainer> > > tracks_per_jet(jets.size());
  for(std::pair<const xAOD::TrackParticle*, const xAOD::Jet*> trkpair : closest_jet_to_track) {
    // If a jet exists (was in dR range), then add the track for that jet
    if(trkpair.second!= nullptr) {
      // Most efficient constructor is with container + index
      tracks_per_jet[trkpair.second->index()].emplace_back(ElementLink<xAOD::IParticleContainer>(trkpair.first,*handle_trk));
      ATH_MSG_VERBOSE("Added track " << trkpair.first->index() << " to association list for jet " << trkpair.second->index());
      ATH_MSG_VERBOSE("Verify ElementLink -- track pt: " << trkpair.first->pt() << "; EL pt; " << (*tracks_per_jet[trkpair.second->index()].back())->pt());
    }
  }

  SG::WriteDecorHandle<xAOD::JetContainer, std::vector<ElementLink<xAOD::IParticleContainer> > > assocLinkHandle(m_decorDRTracks);

  // Now add associated tracks to jet
  for(const xAOD::Jet* jet : jets) {
    assocLinkHandle(*jet) = tracks_per_jet[jet->index()];
    ATH_MSG_DEBUG("Jet " << jet->index() << " has " << tracks_per_jet[jet->index()].size()
                  << " associated tracks with " << m_dRcut);
    if(msgLvl(MSG::VERBOSE)) {
      std::vector<const xAOD::TrackParticle*> tracks;
      bool havetracks = jet->getAssociatedObjects("DRTrack", tracks);
      if(!havetracks) ATH_MSG_WARNING("Associated tracks not found");
      msg(MSG::VERBOSE) << "Retrieved " << tracks.size() << " DRTracks" << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode JetDRTrackAssocTool::fillTrackToJetMap(std::map<const xAOD::TrackParticle*, const xAOD::Jet*>& closest_jet_to_track, 
                       const xAOD::TrackParticleContainer& tracks, const xAOD::JetContainer& jets, float dRcut) const
{
  // Find closest jet for every track
  // Skip jets that are outside the target matching radius -- could configure this from the jet parameters
  for(const xAOD::TrackParticle* track : tracks) {
    if(track==nullptr) {return StatusCode::FAILURE;}
    float mindR2 = 1e6; // large number
    const xAOD::Jet* p_closestJet = nullptr;
    for(const xAOD::Jet* jet : jets) {
      if(jet==nullptr) {return StatusCode::FAILURE;}
      float deltaR2 = xAOD::P4Helpers::deltaR2(*track,*jet);
      ATH_MSG_VERBOSE("Track " << track->index() << " / jet " << jet->index() << " has dR = " << sqrt(deltaR2));
      if(deltaR2<dRcut*dRcut) {
        if(deltaR2<mindR2*mindR2) {
          mindR2 = deltaR2;
          p_closestJet = jet;
          ATH_MSG_VERBOSE("  This is the closest jet to the track, with mindR = " << sqrt(mindR2));
        } // found a closer jet
      } // jet inside dR cone
    } // loop over jets

    // Add pointer to map, even if null
    closest_jet_to_track[track] = p_closestJet;
  }

  return StatusCode::SUCCESS;  
}
