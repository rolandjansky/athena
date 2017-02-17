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
  m_doMuons(true),
  m_muonsKey("Muons"),
  m_electronsKey("Electrons"),
  m_CombinedMuonsTracksKey("CombinedMuonTracks"),
  m_GSFTracksKey("GSFTracks")
{
    
  declareProperty("ThinningSvc", m_thinningSvc,
		  "The ThinningSvc instance for a particular output stream" );

  declareProperty("doElectrons", m_doElectrons,
		  "Flag to Thin the Electron GSF tracks");

  declareProperty("doMuons", m_doMuons,
		  "Flag to Thin the Muon Combinedtracks");

  declareProperty("MuonsKey", m_muonsKey,
		  "StoreGate key for muons container");

  declareProperty("ElectronsKey",m_electronsKey ,
		  "StoreGate key for electrons container");

  declareProperty("CombinedMuonsTrackKey", m_CombinedMuonsTracksKey,
		  "StoreGate key for combined muons Trk::Track container");

  declareProperty("GSFTrackKey", m_GSFTracksKey,
		  "StoreGate key for GSF Trk::Track container");


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
      ATH_MSG_INFO("Will thin " << m_electronsKey << " Trk::Tracks with key " << m_GSFTracksKey);
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
  
    if (m_doElectrons) {
      CHECK(doElectrons());
    }
    if (m_doMuons) {
      CHECK(doMuons());
    }         
    return StatusCode::SUCCESS;
}

StatusCode ThinTrkTrackAlg::doElectrons(){

  //Prepare for the Thinning of Trk::Tracks
  IThinningSvc::VecFilter_t keptTracks;
  const TrackCollection* trackPC(0);
  if(evtStore()->contains<TrackCollection >(m_GSFTracksKey)){
    ATH_CHECK(evtStore()->retrieve(trackPC,m_GSFTracksKey));
  }
  else{
    ATH_MSG_WARNING ("No " << m_GSFTracksKey << " found , disabling electrons");
    m_doElectrons=false;
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("Number of " << m_GSFTracksKey << " " << trackPC->size());
  if( keptTracks.size() < trackPC->size() ) {
    keptTracks.resize( trackPC->size(), false );
  }

  //Get the electrons
  const xAOD::ElectronContainer* electrons =
    evtStore()->retrieve< const xAOD::ElectronContainer >(m_electronsKey);
  if( ! electrons ) {
    ATH_MSG_ERROR( "Couldn't retrieve electron container with key: "<< m_electronsKey);
    return StatusCode::FAILURE;
  }

  //Loop over electrons
  size_t kept(0);
  for(auto el: *electrons) {
    const xAOD::TrackParticle* tp=el->trackParticle();
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
