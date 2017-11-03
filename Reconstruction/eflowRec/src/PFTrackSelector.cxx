#include "eflowRec/eflowTrackExtrapolatorBaseAlgTool.h"
#include "eflowRec/PFTrackSelector.h"
#include "xAODEgamma/ElectronxAODHelpers.h"


PFTrackSelector::PFTrackSelector(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator)
{
}

StatusCode PFTrackSelector::initialize(){

  ATH_CHECK(m_theTrackExtrapolatorTool.retrieve());  
  ATH_CHECK(m_trackSelectorTool.retrieve());

  ATH_CHECK(m_tracksReadHandleKey.initialize());
  ATH_CHECK(m_electronsReadHandleKey.initialize());
  ATH_CHECK(m_muonsReadHandleKey.initialize());

  ATH_CHECK(m_eflowRecTracksWriteHandleKey.initialize());
  
  return StatusCode::SUCCESS;

}

StatusCode PFTrackSelector::execute(){

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
      /* Create the eflowRecCluster and put it in the container */
      std::unique_ptr<eflowRecTrack> thisEFRecTrack  = std::make_unique<eflowRecTrack>(ElementLink<xAOD::TrackParticleContainer>(*tracksReadHandle, trackIndex), m_theTrackExtrapolatorTool);
      thisEFRecTrack->setTrackId(trackIndex);
      eflowRecTracksWriteHandle->push_back(std::move(thisEFRecTrack));
    }
    trackIndex++;
  }

  std::sort(eflowRecTracksWriteHandle->begin(), eflowRecTracksWriteHandle->end(), eflowRecTrack::SortDescendingPt());

  return StatusCode::SUCCESS;
}

StatusCode PFTrackSelector::finalize(){return StatusCode::SUCCESS;}

bool PFTrackSelector::selectTrack(const xAOD::TrackParticle& track) {
  if (track.pt()*0.001 < m_upperTrackPtCut) return m_trackSelectorTool->accept(track, track.vertex());
  else return false;
}

bool PFTrackSelector::isElectron(const xAOD::TrackParticle* track){

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
	    return false;
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
