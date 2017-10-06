#include "eflowRec/eflowTrackExtrapolatorBaseAlgTool.h"
#include "eflowRec/PFTrackSelector.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "xAODEgamma/ElectronxAODHelpers.h"


PFTrackSelector::PFTrackSelector(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_tracksReadHandle("InDetTrackParticles"),
  m_electronsReadHandle("eflowRec_selectedElectrons"),
  m_muonsReadHandle("eflowRec_selectedMuons"),
  m_eflowRecTracksWriteHandle("eflowRecTracks"),
  m_theTrackExtrapolatorTool("Trk::ParticleCaloExtensionTool",this),
  m_upperTrackPtCut(100.0)
{
  declareProperty("tracksName", m_tracksReadHandle);
  declareProperty("electronsName", m_electronsReadHandle);  
  declareProperty("muonsName",  m_muonsReadHandle);
  declareProperty("eflowRecTracksOutputName",  m_eflowRecTracksWriteHandle);
  declareProperty("trackExtrapolatorTool", m_theTrackExtrapolatorTool, "AlgTool to use for track extrapolation");
  declareProperty("trackSelectionTool", m_trackSelectorTool);
  declareProperty("upperTrackPtCut",m_upperTrackPtCut);
}

StatusCode PFTrackSelector::initialize(){

  ATH_CHECK(m_theTrackExtrapolatorTool.retrieve());  
  ATH_CHECK(m_trackSelectorTool.retrieve());

  ATH_CHECK(m_tracksReadHandle.initialize());
  ATH_CHECK(m_electronsReadHandle.initialize());
  ATH_CHECK(m_muonsReadHandle.initialize());

  ATH_CHECK(m_eflowRecTracksWriteHandle.initialize());
  
  return StatusCode::SUCCESS;

}

StatusCode PFTrackSelector::execute(){

  ATH_CHECK(m_eflowRecTracksWriteHandle.record(std::make_unique<eflowRecTrackContainer>()));

  /* Verify the read handle has a valid pointer, and if not return */
  if (!m_tracksReadHandle.isValid()){
    ATH_MSG_WARNING("Can not retrieve xAOD::TrackParticleContainer with name: " << m_tracksReadHandle.key());
    return StatusCode::FAILURE;
  }

  /* Do the track selection for tracks to be used in all of the following steps: */
  int trackIndex = 0;
  for (auto thisTrack : *m_tracksReadHandle){

    if (!thisTrack){
      ATH_MSG_WARNING("Have invalid pointer to xAOD::TrackParticle");
      continue;	
    }

    ATH_MSG_DEBUG("Have track with pt, eta and phi of " << thisTrack->pt() << ", " << thisTrack->eta() << " and " << thisTrack->phi());
    
    bool rejectTrack(!selectTrack(*thisTrack));

    bool isElectron = this->isElectron(thisTrack);
    bool isMuon = this->isMuon(thisTrack);
    ATH_MSG_DEBUG("isElectron is " << isElectron << " and isMuon is " << isMuon);
    if (true == isElectron || true == isMuon) rejectTrack = true;

    ATH_MSG_DEBUG("rejectTrack is " << rejectTrack);
    
    if (!rejectTrack) {
      /* Create the eflowRecCluster and put it in the container */
      std::unique_ptr<eflowRecTrack> thisEFRecTrack  = std::make_unique<eflowRecTrack>(ElementLink<xAOD::TrackParticleContainer>(*m_tracksReadHandle, trackIndex), m_theTrackExtrapolatorTool);
      thisEFRecTrack->setTrackId(trackIndex);
      m_eflowRecTracksWriteHandle->push_back(std::move(thisEFRecTrack));
    }
    trackIndex++;
  }

  std::sort(m_eflowRecTracksWriteHandle->begin(), m_eflowRecTracksWriteHandle->end(), eflowRecTrack::SortDescendingPt());

  return StatusCode::SUCCESS;
}

StatusCode PFTrackSelector::finalize(){return StatusCode::SUCCESS;}

bool PFTrackSelector::selectTrack(const xAOD::TrackParticle& track) {
  if (track.pt()*0.001 < m_upperTrackPtCut) return m_trackSelectorTool->accept(track, track.vertex());
  else return false;
}

bool PFTrackSelector::isElectron(const xAOD::TrackParticle* track){

  if (m_electronsReadHandle.isValid()){

    for (auto thisElectron : *m_electronsReadHandle){

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
  else ATH_MSG_WARNING("Invalid ReadHandle for electrons with key: " << m_electronsReadHandle.key());

  return false;

}

bool PFTrackSelector::isMuon(const xAOD::TrackParticle* track){

  if (m_muonsReadHandle.isValid()){

    for (auto theMuon : *m_muonsReadHandle){      
      if (theMuon){
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
  else ATH_MSG_WARNING("Invalid ReadHandle for muons with key: " << m_muonsReadHandle.key());

  return false;
}
