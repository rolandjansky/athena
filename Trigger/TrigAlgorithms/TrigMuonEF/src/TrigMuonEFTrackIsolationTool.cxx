/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// local includes
#include "TrigMuonEFTrackIsolationTool.h"

// edm includes
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "Particle/TrackParticleContainer.h"

// athena includes
#include "FourMomUtils/P4Helpers.h"

using std::vector;

/**
 * Standard constructor
 */
TrigMuonEFTrackIsolationTool::TrigMuonEFTrackIsolationTool(const std::string& type, const std::string& name, const IInterface* parent) : 
  AthAlgTool( type, name, parent ),
  m_debug(false),
  m_deltaz_cut(0.0),
  m_removeSelf(true),
  m_useAnnulus(false),
  m_annulusSize(-1.0) {
  
  declareInterface<IMuonEFTrackIsolationTool>(this);

  declareProperty("deltaZCut", m_deltaz_cut);
  declareProperty("removeSelf", m_removeSelf);
  declareProperty("useAnnulus", m_useAnnulus);
  declareProperty("annulusSize", m_annulusSize);
}

/**
 * Sum the track pT in cones around the muon.
 *
 * @param efmuon - EF muon
 * @param idtrks - Container of id trks
 * @param conesizes - Vector of conesizes (in dR)
 * @param results - Vector where the results for each cone will be stored
 * @param dzvals - Vector where dz vals of good tracks will be stored
 * @param drvals - Vector where dr vals of good tracks will be stored
 */
StatusCode TrigMuonEFTrackIsolationTool::calcTrackIsolation(const xAOD::Muon* efmuon, const xAOD::TrackParticleContainer* idtrks, const vector<double> conesizes, vector<double>& results, vector<double>* dzvals, vector<double>* drvals) {

  // clear results vector
  results.clear();

  // this will point to the muon (combined or extrapolated)
  const xAOD::TrackParticle* muon=0;

  // variable to store pt of the muon
  double selfpt=0.0;

  //for combined muons use the associated ID track for the self removal
  const xAOD::TrackParticle* muon_idtrk = 0;
  const Trk::Perigee* muidtrk_perigee = 0;
  if( efmuon->combinedTrackParticleLink().isValid()  ) {
    if(m_debug) {
      msg() << MSG::DEBUG << "EF muon has combined muon" << endreq;
    }
    muon = *(efmuon->combinedTrackParticleLink());
    if( efmuon->inDetTrackParticleLink().isValid() ) {
      muon_idtrk = *(efmuon->inDetTrackParticleLink());
      muidtrk_perigee = &(muon_idtrk->perigeeParameters());
      selfpt = muon_idtrk->pt();
      if(m_debug) {
	msg() << MSG::DEBUG << "Found ID track attached to combined muon, " << muon_idtrk << ",pt = " << selfpt << endreq;
      }
    }
    else {
      msg() << MSG::WARNING << "Found EF combined muon without a link to ID track, will use combined pt for self removal" << endreq;
      selfpt = muon->pt();
    }
  }//combinedmuon
  else {
    // for extrapolated muons use the extrapolated muon for self removal
    if( !efmuon->muonSpectrometerTrackParticleLink().isValid() ) {
      if(m_removeSelf) {
	msg() << MSG::WARNING << "This EF muon has neither a combined or extrapolated muon and removeSelf is requested, do not process further" << endreq;
	return StatusCode::FAILURE;
      }//m_removeSelf
      msg() << MSG::WARNING << "This EF muon has neither a combined, extrapolated or MS muon, do not process further" << endreq;
      return StatusCode::FAILURE; 
    }
    else {
      muon = *(efmuon->muonSpectrometerTrackParticleLink());
      selfpt = muon->pt();
    }//extrapmuon
  }//not combined
  
  /// consistency checks
  if(m_removeSelf && selfpt==0.0) {
    msg() << MSG::WARNING << "Inconsistency, removeSelf requested, but selfpt = 0" << endreq;
  }
  if(!muon) {
    msg() << MSG::WARNING << "Could not find a muon to update with the isolation, skipping this muon" << endreq;
    return StatusCode::FAILURE;
  }

  // fill results vector with 0
  results.insert(results.end(), conesizes.size(), 0.0);
  
  // loop over other ID tracks in the container
  for(xAOD::TrackParticleContainer::const_iterator trkit=idtrks->begin();
      trkit!=idtrks->end(); ++trkit) {

    // check dZ if necessary
    if(m_deltaz_cut > 0.0 && muidtrk_perigee) {
      const Trk::Perigee& idtrk_perigee = (*trkit)->perigeeParameters();
      const double dz = idtrk_perigee.parameters()[Trk::z0] - muidtrk_perigee->parameters()[Trk::z0];
      if( fabs(dz) > m_deltaz_cut ) {
	if(m_debug) {
	  msg() << MSG::DEBUG << "ID track failed dz cut, ignoring it. dz = " << dz << endreq;
	}
	continue;
      }//failed delta(z)
      // store dz (after cut)
      if(dzvals) dzvals->push_back(dz);
      if(m_debug) {
	msg() << MSG::DEBUG << "ID track passes dz cut. dz = " << dz << endreq;
      }
      
    }//deltaz_cut
 
    
   
    // check if trk within cone
    double dr = 0;
    if(muon_idtrk) { //use ID track for dR if available
      dr = (*trkit)->p4().DeltaR( muon_idtrk->p4() );
    } else { //use the muon
      dr = (*trkit)->p4().DeltaR( muon->p4() );
    }
    
    if(drvals) drvals->push_back(dr);
    
    // if needed check the inner annululs
    if(m_useAnnulus) {
      if( dr < m_annulusSize ) {
	if(m_debug) {
	  msg() << MSG::DEBUG << "ID track within annulus, ignoring it, dR = " << dr << ", annulus = " << m_annulusSize << endreq;
	 
	}
	continue; // skip tracks in the annulus
      }//annulus cut
      if(m_debug) {
	msg() << MSG::DEBUG << "ID track outside annulus, keep it, dR = " << dr << ", annulus = " << m_annulusSize << endreq;
      }
    }//use Annulus

    // add trk pT to relevant results vector
    for(unsigned int conepos=0; conepos<conesizes.size(); ++conepos) {
      if(dr < conesizes.at(conepos) ) {
	if(m_debug) {
	  msg() << MSG::DEBUG << "Adding trk pt = " << (*trkit)->pt() << ", with dr = " << dr << ", into cone = " << conesizes.at(conepos) << endreq; 
	}
	results.at(conepos) += (*trkit)->pt();
      }
    }
    
  }//ID track loop
  
  if(m_debug) {
    for(unsigned int conepos=0; conepos<conesizes.size(); ++conepos) {
      msg() << MSG::DEBUG << "Scalar pT sum of tracks around this muon cone " << conesizes.at(conepos) << " = " << results.at(conepos) << endreq;
    }
  }
  if(m_removeSelf && !m_useAnnulus) { // remove muon pt from the sums
    for(unsigned int conepos=0; conepos<conesizes.size(); ++conepos) {
      results.at(conepos) -= selfpt;
    }
    if(m_debug) {
      for(unsigned int conepos=0; conepos<conesizes.size(); ++conepos) {
	msg() << MSG::DEBUG << "Scalar pT sum of tracks around this muon cone " << conesizes.at(conepos) << " after self removal = " << results.at(conepos) << endreq;
      }
    }
    
  }//m_removeSelf    
  
  return StatusCode::SUCCESS;
}

/**
 * Sum the track pT in cones around the muon.
 *
 * @param efmuon - EF muon track
 * @param idtrks - Container of id trks
 * @param conesizes - Vector of conesizes (in dR)
 * @param results - Vector where the results for each cone will be stored
 * @param dzvals - Vector where dz vals of good tracks will be stored
 * @param drvals - Vector where dr vals of good tracks will be stored
 */
StatusCode TrigMuonEFTrackIsolationTool::calcTrackIsolation(const TrigMuonEFInfoTrack* efmuon, const Rec::TrackParticleContainer* idtrks, const vector<double> conesizes, vector<double>& results, vector<double>* dzvals, vector<double>* drvals) {

  // clear results vector
  results.clear();

  // this will point to the muon (combined or extrapolated)
  const TrigMuonEFTrack* muon=0;

  // variable to store pt of the muon
  double selfpt=0.0;

  //for combined muons use the associated ID track for the self removal
  const Rec::TrackParticle* muon_idtrk = 0;
  const Trk::Perigee* muidtrk_perigee = 0;
  if( efmuon->hasCombinedTrack() ) {
    if(m_debug) {
      msg() << MSG::DEBUG << "EF muon has combined muon" << endreq;
    }
    const TrigMuonEFCbTrack* cbmuon = efmuon->CombinedTrack();
    muon = efmuon->CombinedTrack();
    if( cbmuon->getIDTrackParticle() ) {
      muon_idtrk = cbmuon->getIDTrackParticle();
      muidtrk_perigee = muon_idtrk->measuredPerigee();
      selfpt = muon_idtrk->pt();
      if(m_debug) {
	msg() << MSG::DEBUG << "Found ID track attached to combined muon, pt = " << selfpt << endreq;
      }
    }
    else {
      msg() << MSG::WARNING << "Found EF combined muon without a link to ID track, will use combined pt for self removal" << endreq;
      selfpt = cbmuon->pt();
    }
  }//combinedmuon
  else {
    // for extrapolated muons use the extrapolated muon for self removal
    if(!efmuon->hasExtrapolatedTrack()) {
      if(m_removeSelf) {
	msg() << MSG::WARNING << "This EF muon has neither a combined or extrapolated muon and removeSelf is requested, do not process further" << endreq;
	return StatusCode::FAILURE;
      }//m_removeSelf
      else { //use spectrometer track
	
	if(!efmuon->hasSpectrometerTrack()) {
	  msg() << MSG::WARNING << "This EF muon has neither a combined, extrapolated or MS muon, do not process further" << endreq;
	  return StatusCode::FAILURE;
	} 
	//const TrigMuonEFTrack* msmuon = efmuon->SpectrometerTrack();
	muon = efmuon->SpectrometerTrack();
      }
    }
    else {
      const TrigMuonEFTrack* extrapmuon = efmuon->ExtrapolatedTrack();
      selfpt = extrapmuon->pt();
      muon = efmuon->ExtrapolatedTrack();
    }//extrapmuon
  }//not combined
  
  /// consistency checks
  if(m_removeSelf && selfpt==0.0) {
    msg() << MSG::WARNING << "Inconsistency, removeSelf requested, but selfpt = 0" << endreq;
  }
  if(!muon) {
    msg() << MSG::WARNING << "Could not find a muon to update with the isolation, skipping this muon" << endreq;
    return StatusCode::FAILURE;
  }

  // fill results vector with 0
  results.insert(results.end(), conesizes.size(), 0.0);
  
  // loop over other ID tracks in the container
  for(Rec::TrackParticleContainer::const_iterator trkit=idtrks->begin();
      trkit!=idtrks->end(); ++trkit) {

    // check dZ if necessary
    if(m_deltaz_cut > 0.0 && muidtrk_perigee) {
      const Trk::Perigee* idtrk_perigee = (*trkit)->measuredPerigee();
      if(idtrk_perigee) { 
	const double dz = idtrk_perigee->parameters()[Trk::z0] - muidtrk_perigee->parameters()[Trk::z0];
	if( fabs(dz) > m_deltaz_cut ) {
	  if(m_debug) {
	    msg() << MSG::DEBUG << "ID track failed dz cut, ignoring it. dz = " << dz << endreq;
	  }
	  continue;
	}//failed delta(z)
	// store dz (after cut)
	if(dzvals) dzvals->push_back(dz);
	if(m_debug) {
	  msg() << MSG::DEBUG << "ID track passes dz cut. dz = " << dz << endreq;
	}
      } else {
	msg() << MSG::WARNING << "Can not make delta(z) cut because id trk has invalid measuredPerigee. This track will be included in the isolation calculation" << endreq;
      }
      
    }//deltaz_cut
 
    
   
    // check if trk within cone
    double dr = 0;
    if(muon_idtrk) { //use ID track for dR if available
      dr = P4Helpers::deltaR( **trkit, muon_idtrk->eta(), muon_idtrk->phi());
    } else { //use the muon
      dr = P4Helpers::deltaR( **trkit, muon->eta(), muon->phi());
    }
    
    if(drvals) drvals->push_back(dr);

    // if needed check the inner annululs
    if(m_useAnnulus) {
      if( dr < m_annulusSize ) {
	if(m_debug) {
	  msg() << MSG::DEBUG << "ID track within annulus, ignoring it, dR = " << dr << ", annulus = " << m_annulusSize << endreq;
	 
	}
	continue; // skip tracks in the annulus
      }//annulus cut
      if(m_debug) {
	msg() << MSG::DEBUG << "ID track outside annulus, keep it, dR = " << dr << ", annulus = " << m_annulusSize << endreq;
      }
    }//use Annulus

    // add trk pT to relevant results vector
    for(unsigned int conepos=0; conepos<conesizes.size(); ++conepos) {
      if(dr < conesizes.at(conepos) ) {
	if(m_debug) {
	  msg() << MSG::DEBUG << "Adding trk pt = " << (*trkit)->pt() << ", with dr = " << dr << ", into cone = " << conesizes.at(conepos) << endreq; 
	}
	results.at(conepos) += (*trkit)->pt();
      }
    }
    
  }//ID track loop
  
  if(m_debug) {
    for(unsigned int conepos=0; conepos<conesizes.size(); ++conepos) {
      msg() << MSG::DEBUG << "Scalar pT sum of tracks around this muon cone " << conesizes.at(conepos) << " = " << results.at(conepos) << endreq;
    }
  }
  if(m_removeSelf && !m_useAnnulus) { // remove muon pt from the sums
    for(unsigned int conepos=0; conepos<conesizes.size(); ++conepos) {
      results.at(conepos) -= selfpt;
    }
    if(m_debug) {
      for(unsigned int conepos=0; conepos<conesizes.size(); ++conepos) {
	msg() << MSG::DEBUG << "Scalar pT sum of tracks around this muon cone " << conesizes.at(conepos) << " after self removal = " << results.at(conepos) << endreq;
      }
    }
    
  }//m_removeSelf    
  
  return StatusCode::SUCCESS;
}

StatusCode TrigMuonEFTrackIsolationTool::initialize() {

  m_debug = msgLvl(MSG::DEBUG);

  if(m_debug) {
    msg() << MSG::DEBUG << "Initializing TrigMuonEFTrackIsolationTool[" << name() << "]" << endreq;
    msg() << MSG::DEBUG
	  << "package version = " << PACKAGE_VERSION << endreq;
    msg() << MSG::DEBUG
	  << "Properties set as follows: " << endreq;
    msg() << MSG::DEBUG
	  << "removeSelf                     " << m_removeSelf << endreq;
    msg() << MSG::DEBUG
	  << "deltaZCut                      " << m_deltaz_cut << endreq;
    msg() << MSG::DEBUG
	  << "useAnnulus                     " << m_useAnnulus << endreq;
    msg() << MSG::DEBUG
	  << "annulusSize                    " << m_annulusSize << endreq;
    
  }//debug

  // check the annulus size is valid
  if(m_useAnnulus) {
    if(m_annulusSize < 0.0 || m_annulusSize > 0.2) {
      msg() << MSG::ERROR << "Bad configuration of annulusSize = " << m_annulusSize << ", fix your config!" << endreq;
      return StatusCode::FAILURE;
    }
  }//useAnnulus

  return StatusCode::SUCCESS;
}

StatusCode TrigMuonEFTrackIsolationTool::finalize() {

  return StatusCode::SUCCESS;
}
