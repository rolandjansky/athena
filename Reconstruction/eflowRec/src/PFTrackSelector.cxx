/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "eflowRec/eflowTrackExtrapolatorBaseAlgTool.h"
#include "eflowRec/PFTrackSelector.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "xAODEgamma/ElectronxAODHelpers.h"
#include "GaudiKernel/SystemOfUnits.h"

constexpr float invGeV = 1./CLHEP::GeV;

PFTrackSelector::PFTrackSelector(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator)
{
}

StatusCode PFTrackSelector::initialize(){

  ATH_CHECK(m_theTrackExtrapolatorTool.retrieve());  
  ATH_CHECK(m_trackSelectorTool.retrieve());

  ATH_CHECK(m_tracksReadHandleKey.initialize());
  ATH_CHECK(m_vertexKey.initialize());
  ATH_CHECK(m_pixelDetEleCollKey.initialize());
  ATH_CHECK(m_SCTDetEleCollKey.initialize());
  ATH_CHECK(m_trtDetEleContKey.initialize());

  // Optional readhandlekeys for electrons and muons
  if(!m_electronsReadHandleKey.key().empty()) {
    ATH_CHECK(m_electronsReadHandleKey.initialize());
  }
  if(!m_muonsReadHandleKey.key().empty()) {
    ATH_CHECK(m_muonsReadHandleKey.initialize());
  }

  ATH_CHECK(m_eflowRecTracksWriteHandleKey.initialize());
  
  if (!m_monTool.empty()) ATH_CHECK(m_monTool.retrieve());

  return StatusCode::SUCCESS;

}

StatusCode PFTrackSelector::execute(){
  // Monitor the time taken to execute the alg
  auto t_exec = Monitored::Timer<std::chrono::milliseconds>( "TIME_execute" );
  auto N_tracks = Monitored::Scalar( "N_tracks", 0 );
  auto eta_track = Monitored::Scalar( "eta_track", 0. );
  auto pt_track = Monitored::Scalar( "pt_track", 0. );

  SG::WriteHandle<eflowRecTrackContainer> eflowRecTracksWriteHandle(m_eflowRecTracksWriteHandleKey);  
  ATH_CHECK(eflowRecTracksWriteHandle.record(std::make_unique<eflowRecTrackContainer>()));

  /* Verify the read handle has a valid pointer, and if not return */
  SG::ReadHandle<xAOD::TrackParticleContainer> tracksReadHandle(m_tracksReadHandleKey);
  if (!tracksReadHandle.isValid()){
    ATH_MSG_WARNING("Can not retrieve xAOD::TrackParticleContainer with name: " << tracksReadHandle.key());
    return StatusCode::FAILURE;
  }

  /* Do the track selection for tracks to be used in all of the following steps: */
  int trackIndex = 0;
  for (auto thisTrack : *tracksReadHandle){

    if (!thisTrack){
      ATH_MSG_WARNING("Have invalid pointer to xAOD::TrackParticle");
      continue;	
    }

    ATH_MSG_DEBUG("Have track with E, pt, eta and phi of " << thisTrack->e() << ", " << thisTrack->pt() << ", " << thisTrack->eta() << " and " << thisTrack->phi());
    
    bool rejectTrack(!selectTrack(*thisTrack));

    bool isElectron = this->isElectron(thisTrack);
    bool isMuon = this->isMuon(thisTrack);
    ATH_MSG_DEBUG("isElectron is " << isElectron << " and isMuon is " << isMuon);
    if (true == isElectron || true == isMuon) rejectTrack = true;

    ATH_MSG_DEBUG("rejectTrack is " << rejectTrack);
    
    if (!rejectTrack) {
      // Monitor the time per selected track
      auto t_track = Monitored::Timer<std::chrono::microseconds>( "TIME_track" );
      /* Create the eflowRecCluster and put it in the container */
      std::unique_ptr<eflowRecTrack> thisEFRecTrack  = std::make_unique<eflowRecTrack>(ElementLink<xAOD::TrackParticleContainer>(*tracksReadHandle, trackIndex), m_theTrackExtrapolatorTool);
      thisEFRecTrack->setTrackId(trackIndex);
      eflowRecTracksWriteHandle->push_back(std::move(thisEFRecTrack));
      eta_track = thisTrack->eta();
      pt_track = thisTrack->pt() * invGeV;
      // Fill histogram
      auto mon_trk = Monitored::Group(m_monTool, t_track, eta_track, pt_track);
    }
    trackIndex++;
  }

  std::sort(eflowRecTracksWriteHandle->begin(), eflowRecTracksWriteHandle->end(), eflowRecTrack::SortDescendingPt());

  N_tracks = eflowRecTracksWriteHandle->size();
  auto mon_exectime = Monitored::Group(m_monTool, t_exec, N_tracks);
  return StatusCode::SUCCESS;
}

StatusCode PFTrackSelector::finalize(){return StatusCode::SUCCESS;}

bool PFTrackSelector::selectTrack(const xAOD::TrackParticle& track) {
  //We pass a null xAOD::Vertex pointer to accept here, the vertex is only needed if z0 cuts are applied (which they are not)
  if (track.pt()*0.001 < m_upperTrackPtCut) return static_cast<bool>(m_trackSelectorTool->accept(track, nullptr));
  else return false;
}

bool PFTrackSelector::isElectron(const xAOD::TrackParticle* track){

  if(m_electronsReadHandleKey.key().empty()) return false;

  SG::ReadHandle<xAOD::ElectronContainer> electronsReadHandle(m_electronsReadHandleKey);
  if (electronsReadHandle.isValid()){

    for (auto thisElectron : *electronsReadHandle){

      if (thisElectron){
	unsigned int nTrack = thisElectron->nTrackParticles();
	
	if (0 != nTrack){	  
	  const xAOD::TrackParticle* origTrack = xAOD::EgammaHelpers::getOriginalTrackParticle(thisElectron);	  
	  if (origTrack){
	    if (track == origTrack) {
	      return true;
	    }
	  }//if valid track pointer
	  else ATH_MSG_WARNING("Electron object map has NULL pointer to original TrackParticle");
	}//if has a track
	else ATH_MSG_WARNING("Electron object has " << nTrack << " tracks");
      }//if valid pointer
      else ATH_MSG_WARNING("Electron is a NULL pointer");
    }//electron loop    
  }
  else ATH_MSG_WARNING("Invalid ReadHandle for electrons with key: " << electronsReadHandle.key());

  return false;

}

bool PFTrackSelector::isMuon(const xAOD::TrackParticle* track){

  if(m_muonsReadHandleKey.key().empty()) return false;

  SG::ReadHandle<xAOD::MuonContainer> muonsReadHandle(m_muonsReadHandleKey);
  if (muonsReadHandle.isValid()){

    for (auto theMuon : *muonsReadHandle){      
      if (theMuon){
	ATH_MSG_DEBUG("Considering muon in isMuon with e,pt, eta and phi of " << theMuon->e() << ", " << theMuon->pt() << ", " << theMuon->eta() << " and " << theMuon->phi());
	const ElementLink< xAOD::TrackParticleContainer > theLink = theMuon->inDetTrackParticleLink();
	if (theLink.isValid()){
	  const xAOD::TrackParticle* ID_track = *theLink;
	  if (ID_track){
	    if (track == ID_track) return true;
	  }
	  else ATH_MSG_WARNING("This muon has a NULL pointer to the track");
	}
	else ATH_MSG_WARNING("This muon has an invalid link to the track");
      }//if muon pointer is valid
      else ATH_MSG_WARNING("This muon is a NULL pointer");
    }//muon loop
  }
  else ATH_MSG_WARNING("Invalid ReadHandle for muons with key: " << muonsReadHandle.key());

  return false;
}
