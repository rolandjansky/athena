///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ThinTrkTrackAlg.cxx
// Author: Christos
// Lets see if we can write Trk::Tracks in xAOD
///////////////////////////////////////////////////////////////////

#include "ThinTrkTrackAlg.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODEgamma/ElectronxAODHelpers.h"
#include "TrkTrack/TrackCollection.h"
#include "AthenaKernel/IThinningSvc.h"

// STL includes
#include <algorithm> 

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
ThinTrkTrackAlg::ThinTrkTrackAlg( const std::string& name,
				  ISvcLocator* pSvcLocator ) :
  ::AthAlgorithm( name, pSvcLocator ),
  m_thinningSvc( "ThinningSvc/ThinningSvc", name ),
  m_doElectrons(true),
  m_doPhotons(true),
  m_doMuons(true),
  m_muonsKey("Muons"),
  m_electronsKey("Electrons"),
  m_photonsKey("Photons"),
  m_CombinedMuonsTracksKey("CombinedMuonTracks"),
  m_GSFTracksKey("GSFTracks"),
  m_minptElectrons(4000.),
  m_minptPhotons(4000.),
  m_minptMuons(4000.),
  m_bestonlyElectrons(true),
  m_bestonlyPhotons(true)
{
    
  declareProperty("ThinningSvc", m_thinningSvc,
		  "The ThinningSvc instance for a particular output stream" );

  declareProperty("doElectrons", m_doElectrons,
		  "Flag to Thin the Electron GSF tracks");

  declareProperty("doPhotons", m_doPhotons,
		  "Flag to Thin the Photon GSF tracks");

  declareProperty("doMuons", m_doMuons,
		  "Flag to Thin the Muon Combinedtracks");

  declareProperty("MuonsKey", m_muonsKey,
		  "StoreGate key for muons container");

  declareProperty("ElectronsKey",m_electronsKey ,
		  "StoreGate key for electrons container");

  declareProperty("PhotonsKey",m_photonsKey ,
		  "StoreGate key for photon container");

  declareProperty("CombinedMuonsTrackKey", m_CombinedMuonsTracksKey,
		  "StoreGate key for combined muons Trk::Track container");

  declareProperty("GSFTrackKey", m_GSFTracksKey,
		  "StoreGate key for GSF Trk::Track container");

  declareProperty("minptElectrons", m_minptElectrons,
		  "Minimum pT for electrons to have their tracks included");

  declareProperty("minptPhotons", m_minptPhotons,
		  "Minimum pT for photons to have their tracks included");

  declareProperty("minptMuons", m_minptMuons,
		  "Minimum pT for muons to have their tracks included");

  declareProperty("OnlyBestElectrons", m_bestonlyElectrons,
		  "Include only the best match for electrons");

  declareProperty("OnlyBestPhotons", m_bestonlyPhotons,
		  "Include only the best match for photons");


}

// Destructor
///////////////
ThinTrkTrackAlg::~ThinTrkTrackAlg()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ThinTrkTrackAlg::initialize(){
    ATH_MSG_DEBUG ("Initializing " << name() << "...");

    if ( !m_thinningSvc.retrieve().isSuccess() ) {
       ATH_MSG_ERROR("Unable to retrieve pointer to IThinningSvc");
       return StatusCode::FAILURE;
    }
    // Print out the used configuration
    ATH_MSG_DEBUG ( " using = " << m_thinningSvc );

    // Is truth thinning required?
    if (m_doElectrons) {
      ATH_MSG_INFO("Will thin " << m_electronsKey << " Trk::Tracks with key (electrons) " << m_GSFTracksKey);
    }
    if (m_doPhotons) {
      ATH_MSG_INFO("Will thin " << m_photonsKey << " Trk::Tracks with key (photons) " << m_GSFTracksKey);
    }
    if (m_doMuons) {
      ATH_MSG_INFO("Will thin " << m_muonsKey << " Trk::Tracks with key " << m_CombinedMuonsTracksKey);
    }     
    ATH_MSG_DEBUG ( "==> done with initialize " << name() << "..." );
    return StatusCode::SUCCESS;
}



StatusCode ThinTrkTrackAlg::finalize(){
    ATH_MSG_DEBUG ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
}

StatusCode ThinTrkTrackAlg::execute(){
  
    if (m_doElectrons || m_doPhotons) {
      CHECK(doEGamma());
    }
    if (m_doMuons) {
      CHECK(doMuons());
    }         
    return StatusCode::SUCCESS;
}

StatusCode ThinTrkTrackAlg::doEGamma(){

  //Prepare for the Thinning of Trk::Tracks
  IThinningSvc::VecFilter_t keptTracks;
  const TrackCollection* trackPC(0);
  if(evtStore()->contains<TrackCollection >(m_GSFTracksKey)){
    ATH_CHECK(evtStore()->retrieve(trackPC,m_GSFTracksKey));
  }
  else{
    ATH_MSG_WARNING ("No " << m_GSFTracksKey << " found , disabling electrons and photons");
    m_doElectrons=false;
    m_doPhotons=false;
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("Number of " << m_GSFTracksKey << " " << trackPC->size());
  if( keptTracks.size() < trackPC->size() ) {
    keptTracks.resize( trackPC->size(), false );
  }

  size_t kept(0);

  if(m_doElectrons){
    //Get the electrons
    const xAOD::ElectronContainer* electrons =
      evtStore()->retrieve< const xAOD::ElectronContainer >(m_electronsKey);
    if( ! electrons ) {
      ATH_MSG_ERROR( "Couldn't retrieve electron container with key: "<< m_electronsKey);
      return StatusCode::FAILURE;
    }
    
    //Loop over electrons
    for(auto el: *electrons) {
      if(el->pt() < m_minptElectrons){ 
	continue;
      }
      size_t nel = el->nTrackParticles();
      if(m_bestonlyElectrons && nel > 1){ 
	nel = 1;
      }
      for(size_t i=0; i<nel; i++){
	const xAOD::TrackParticle* tp=el->trackParticle(i);
	if ( ! tp || !tp->trackLink().isValid() ){
	  continue;
	}
	size_t index=  tp->trackLink().index();
	keptTracks[index]=true;
	++kept;
      }
    }
  }

  if(m_doPhotons){
    //Get the photons
    const xAOD::PhotonContainer* photons =
      evtStore()->retrieve< const xAOD::PhotonContainer >(m_photonsKey);
    if( ! photons ) {
      ATH_MSG_ERROR( "Couldn't retrieve photon container with key: "<< m_photonsKey);
      return StatusCode::FAILURE;
    }
    
    //Loop over photons
    for(auto ph: *photons) {
      if(ph->pt() < m_minptPhotons){
	continue;
      }
      size_t nvx = ph->nVertices();
      if(m_bestonlyPhotons && nvx > 1){
	nvx = 1;
      }
      for(size_t i=0; i<nvx; i++){
	const xAOD::Vertex* vx=ph->vertex(i);
	if(vx){
	  size_t ntp = vx->nTrackParticles();
	  for(size_t j=0; j<ntp; j++){
	    const xAOD::TrackParticle* tp=vx->trackParticle(j);
	    if ( ! tp || !tp->trackLink().isValid() ){
	      continue;
	    }
	    size_t index=  tp->trackLink().index();
	    keptTracks[index]=true;
	    ++kept;
	  }
	}
      }
    }
  }
    
  ATH_MSG_DEBUG("keep " << kept << " out of " <<keptTracks.size());
  ATH_MSG_DEBUG("Do the Thinning");
  CHECK( m_thinningSvc->filter( *trackPC, keptTracks,IThinningSvc::Operator::Or) );
  return StatusCode::SUCCESS;
}



StatusCode ThinTrkTrackAlg::doMuons(){

  //Prepare for the Thinning of Trk::Tracks
  IThinningSvc::VecFilter_t keptTracks;
  const TrackCollection* trackPC(0);
 
  if(evtStore()->contains<TrackCollection >(m_CombinedMuonsTracksKey)){
    ATH_CHECK(evtStore()->retrieve(trackPC,m_CombinedMuonsTracksKey));
  }
  else{
    ATH_MSG_WARNING ("No " << m_CombinedMuonsTracksKey << " found, disabling Muons");
    m_doMuons=false;
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG("Number of " << m_CombinedMuonsTracksKey << " " << trackPC->size());
  if( keptTracks.size() < trackPC->size() ) {
    keptTracks.resize( trackPC->size(), false );
  }
  
  //Get the muons
  const xAOD::MuonContainer* muons =
    evtStore()->retrieve< const xAOD::MuonContainer >(m_muonsKey);

  if( ! muons ) {
    ATH_MSG_ERROR( "Couldn't retrieve muon container with key: "<< m_muonsKey);
    return StatusCode::FAILURE;
  }

  //Loop over muons
  size_t kept(0);
  for(auto mu: *muons) {
    if(mu->pt() < m_minptMuons){
	continue;
      }
    static SG::AuxElement::Accessor<ElementLink< xAOD::TrackParticleContainer>> acc( "combinedTrackParticleLink" );
    if( ! acc.isAvailable(*mu) ) {
      continue;
    }
    const ElementLink< xAOD::TrackParticleContainer > link = acc(*mu);
    if(! link.isValid() ) {
      continue;
    }
    const xAOD::TrackParticle* tp=(*link);
    if ( ! tp || !tp->trackLink().isValid() ){
      continue;
    }
    size_t index=  tp->trackLink().index();
    keptTracks[index]=true;
    ++kept;
  }

  ATH_MSG_DEBUG("keep " << kept << " out of " <<keptTracks.size());
  ATH_MSG_DEBUG("Do the Thinning");
  CHECK( m_thinningSvc->filter( *trackPC, keptTracks,IThinningSvc::Operator::Or) );
  return StatusCode::SUCCESS;
}
