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
  m_annulusSize(-1.0),
  m_useVarIso(false),
  m_removeSelfType(0) // 0 = Standard self removal, 1 = LeadTrack removal, 2 = dR Matching (1&2 for FTK isolation only)
{  
  
  declareInterface<IMuonEFTrackIsolationTool>(this);

  declareProperty("deltaZCut", m_deltaz_cut);
  declareProperty("removeSelf", m_removeSelf);
  declareProperty("useAnnulus", m_useAnnulus);
  declareProperty("annulusSize", m_annulusSize);
  declareProperty("useVarIso", m_useVarIso);
  declareProperty("removeSelfType", m_removeSelfType);

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
 * @param FTK - boolean stating whether an FTK track collection has been passed
 * @param selfremoval - Vector storing the selfpt and removed pt 
 */

StatusCode TrigMuonEFTrackIsolationTool::calcTrackIsolation(const xAOD::Muon* efmuon, const xAOD::TrackParticleContainer* idtrks, vector<double> conesizes, vector<double>& results, vector<double>* dzvals, vector<double>* drvals, bool FTK, vector<double>* selfremoval) {


  if (m_debug)
    msg() << MSG::DEBUG << "Running isolation over EF Muon!" << endmsg;
  
  // clear results vector
  results.clear();

  // this will point to the muon (combined or extrapolated)
  const xAOD::TrackParticle* muon=0;

 // variable to store pt of the muon (this is different for different muon types so we pass it separately) 
  double selfpt=0.0;

  //for combined muons use the associated ID track for the self removal
  const xAOD::TrackParticle* muon_idtrk = 0;
  const Trk::Perigee* muidtrk_perigee = 0;
  if( efmuon->combinedTrackParticleLink().isValid()  ) {
    if(m_debug) {
      msg() << MSG::DEBUG << "EF muon has combined muon" << endmsg;
    }
    muon = *(efmuon->combinedTrackParticleLink());
    if( efmuon->inDetTrackParticleLink().isValid() ) {
      muon_idtrk = *(efmuon->inDetTrackParticleLink());
      muidtrk_perigee = &(muon_idtrk->perigeeParameters());
      selfpt = muon_idtrk->pt();
      if(m_debug) {
	msg() << MSG::DEBUG << "Found ID track attached to combined muon, " << muon_idtrk << ",pt = " << selfpt << endmsg;
      }
    }
    else {
      msg() << MSG::WARNING << "Found EF combined muon without a link to ID track, will use combined pt for self removal" << endmsg;
      selfpt = muon->pt();
    }
  }//combinedmuon
  else {
    // for extrapolated muons use the extrapolated muon for self removal
    if( !efmuon->muonSpectrometerTrackParticleLink().isValid() ) {
      if(m_removeSelf) {
	msg() << MSG::WARNING << "This EF muon has neither a combined or extrapolated muon and removeSelf is requested, do not process further" << endmsg;
	return StatusCode::FAILURE;
      }//m_removeSelf
      msg() << MSG::WARNING << "This EF muon has neither a combined, extrapolated or MS muon, do not process further" << endmsg;
      return StatusCode::FAILURE; 
    }
    else {
      muon = *(efmuon->muonSpectrometerTrackParticleLink());
      selfpt = muon->pt();
    }//extrapmuon
  }//not combined

 StatusCode isoResult = checkIsolation(muon, selfpt, muon_idtrk, muidtrk_perigee, idtrks, conesizes, results, dzvals, drvals, FTK, selfremoval);

  return isoResult;
}

/**
 *
 * @param L2muon - L2muon
 * @param trks - Container of (id || FTK) trks
 * @param conesizes - Vector of conesizes (in dR)
 * @param results - Vector where the results for each cone will be stored
 * @param dzvals - Vector where dz vals of good tracks will be stored
 * @param drvals - Vector where dr vals of good tracks will be stored
 * @param FTK - boolean stating whether an FTK track collection has been passed
 * @param selfremoval - Vector storing the selfpt and removed pt 
 */
StatusCode TrigMuonEFTrackIsolationTool::calcTrackIsolation(const xAOD::L2CombinedMuon* L2muon, const xAOD::TrackParticleContainer* trks, vector<double> conesizes, vector<double>& results, vector<double>* dzvals, vector<double>* drvals, bool FTK, vector<double>* selfremoval) {

  
  if (m_debug)
    msg() << MSG::DEBUG << "Running isolation over L2Combined Muon!" << endmsg;
  
  // clear results vector
  results.clear();

  // variable to store pt of the muon (this is different for different muon types so we pass it separately)
  double selfpt=0.0;

  //for combined muons use the associated ID track for the self removal
  const xAOD::TrackParticle* muon_idtrk = 0;
  const Trk::Perigee* muidtrk_perigee = 0; 
  if( L2muon->idTrack()  ) {
    if(m_debug) {
      msg() << MSG::DEBUG << "L2 muon has combined muon" << endmsg;
    }
    muon_idtrk = L2muon->idTrack();
    muidtrk_perigee = &(muon_idtrk->perigeeParameters());
    selfpt = muon_idtrk->pt();
    if(m_debug) {
      msg() << MSG::DEBUG << "Found ID track attached to combined muon, " << muon_idtrk << ",pt = " << selfpt << endmsg;
    }
  }//combinedmuon
  else {
    msg() << MSG::WARNING << "L2Combined muon with no ID track? Exiting isolation tool..." << endmsg;
    return StatusCode::FAILURE;

    }//not combined

  StatusCode isoResult = checkIsolation(L2muon, selfpt, muon_idtrk, muidtrk_perigee, trks, conesizes, results, dzvals, drvals, FTK, selfremoval);

  return isoResult;
  
}


// Generic function to take whatever kind of muon and check isolation (so can pass a L2 or EF muon)
StatusCode TrigMuonEFTrackIsolationTool::checkIsolation(const xAOD::IParticle* muon, double selfpt, const xAOD::TrackParticle* muon_idtrk, const Trk::Perigee* muidtrk_perigee, const xAOD::TrackParticleContainer* trks, vector<double> conesizes, vector<double>& results, vector<double>* dzvals, vector<double>* drvals, bool FTK, vector<double>* selfremoval) {
  
  /// consistency checks
  if(m_removeSelf && selfpt==0.0) {
    msg() << MSG::WARNING << "Inconsistency, removeSelf requested, but selfpt = 0" << endmsg;
  }
  if(!muon) {
    msg() << MSG::WARNING << "Could not find a muon to update with the isolation, skipping this muon" << endmsg;
    return StatusCode::FAILURE;
  }

  // fill results vector with 0
  results.insert(results.end(), conesizes.size(), 0.0);
  
  // loop over other tracks in the container
  // these are arrays, so that if using leadTrk or dRMatched selfremoval it can be different between the two cone sizes
  int ntrks_tmp[2];
  ntrks_tmp[0]=0; ntrks_tmp[1]=0;
  
  double leadtrackpt[2], dRmatchedpt[2], dRmatcheddR[2];
  leadtrackpt[0]=0; leadtrackpt[1]=0; dRmatchedpt[0]=0; dRmatchedpt[1]=0; dRmatcheddR[0]=999; dRmatcheddR[1]=999;
  
  if (m_debug && FTK)
    msg() << MSG::DEBUG << "We are running FTKs" << endmsg;
    
  for(xAOD::TrackParticleContainer::const_iterator trkit=trks->begin();
      trkit!=trks->end(); ++trkit) {

    if (m_debug)
      msg() << MSG::DEBUG << "INFO: Track pT = " << (*trkit)->pt() << endmsg;
    
    // check dZ if necessary   
   double dz=0;
   if(m_deltaz_cut > 0.0 && muidtrk_perigee) {
     const Trk::Perigee& idtrk_perigee = (*trkit)->perigeeParameters();
     dz = idtrk_perigee.parameters()[Trk::z0] - muidtrk_perigee->parameters()[Trk::z0];
     if( fabs(dz) > m_deltaz_cut ) {
       if(m_debug) {
	 msg() << MSG::DEBUG << "Track failed dz cut, ignoring it. dz = " << dz << endmsg;
       }
       continue;
     }//failed delta(z)
      // store dz (after cut)
     // if(dzvals) dzvals->push_back(dz); // moved to after the pT cut for plotting purposes
     if(m_debug) {
       msg() << MSG::DEBUG << "ID track passes dz cut. dz = " << dz << endmsg;
     }
      
   }//deltaz_cut
   if(dzvals) dzvals->push_back(dz);
  
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
	  msg() << MSG::DEBUG << "ID track within annulus, ignoring it, dR = " << dr << ", annulus = " << m_annulusSize << endmsg;
	 
	}
	continue; // skip tracks in the annulus
      }//annulus cut
      if(m_debug) {
	msg() << MSG::DEBUG << "ID track outside annulus, keep it, dR = " << dr << ", annulus = " << m_annulusSize << endmsg;
      }
    }//use Annulus

    // Define new conesizes that depend on useVarIso flag
    double ptvarcone20 = std::min(0.2,10000/selfpt); 
    double ptvarcone30 = std::min(0.3,10000/selfpt); 
    if (m_useVarIso){
      conesizes.clear();
      conesizes.push_back(ptvarcone20);
      conesizes.push_back(ptvarcone30);
    }

    // add trk pT to relevant results vector
    for(unsigned int conepos=0; conepos<conesizes.size(); ++conepos) {
      if(dr < conesizes.at(conepos) ) {
	if(m_debug) {
	  msg() << MSG::DEBUG << "Adding trk pt = " << (*trkit)->pt() << ", with dr = " << dr << ", into cone = " << conesizes.at(conepos) << endmsg; 
	}
	results.at(conepos) += (*trkit)->pt();
	++ntrks_tmp[conepos];

	if (FTK){
	  // Lead track 
	  if ((*trkit)->pt() > leadtrackpt[conepos]){
	    leadtrackpt[conepos] = (*trkit)->pt(); 
	    msg() << MSG::DEBUG << "leadtrackpT["<<conepos<<"] changed to= " << leadtrackpt[conepos] << endmsg;
	  }
	  // dR matched track
	  if (dr < dRmatcheddR[conepos]){
	    dRmatchedpt[conepos] = (*trkit)->pt();
	    dRmatcheddR[conepos] = dr;
	  }
	}
      }
    }
    
  }// track loop

  /*---------------------------
    Alternative self removal options
  ---------------------------*/
  double removedpt[2];
  removedpt[0] = selfpt;
  removedpt[1] = selfpt;

  if (FTK){ // only do this if we are running the FTK tracks, stick with default for ID 
    for(unsigned int conepos=0; conepos<conesizes.size(); ++conepos) {
      if (m_debug){
	std::string removalType[3];
	removalType[0] = "Standard"; removalType[1] = "LeadTrk"; removalType[2] = "dRMatch";
	msg() << MSG::DEBUG << "INFO: Self removal for cone of size " << conesizes[conepos] << endmsg;										  
	msg() << MSG::DEBUG << "INFO: Using " << removalType[m_removeSelfType] << " self removal" << endmsg;
      }
      if (m_removeSelfType == 1){
	if (m_debug)
	  msg() << MSG::DEBUG << m_removeSelfType << " self removal - leadtrackpt = " << leadtrackpt[conepos] << ", while selfpt was " << selfpt << endmsg;
	removedpt[conepos] = leadtrackpt[conepos];
      }
      else if (m_removeSelfType == 2){
	if (m_debug)
	  msg() << MSG::DEBUG << m_removeSelfType << " self removal - dRmatcheddR = " << dRmatcheddR[conepos] << ", dRmatchedpt = " << dRmatchedpt[conepos] << ", while selfpt was " << selfpt << endmsg;
	removedpt[conepos] = dRmatchedpt[conepos];
      }
      else{
	if (m_debug)
	  msg() << MSG::DEBUG <<  "Using default self removal. m_removeSelfType = " << m_removeSelfType << endmsg;
      }
    }
  }

  // Muon pT plots
  selfremoval->push_back(selfpt);
  selfremoval->push_back(muon->pt());
  selfremoval->push_back(removedpt[0]);
  

  
  if(m_debug) {
    for(unsigned int conepos=0; conepos<conesizes.size(); ++conepos) {
      msg() << MSG::DEBUG << "Scalar pT sum of tracks around this muon cone " << conesizes.at(conepos) << " = " << results.at(conepos) << endmsg;
    }
  }
  if(m_removeSelf && !m_useAnnulus) { // remove muon pt from the sums
    for(unsigned int conepos=0; conepos<conesizes.size(); ++conepos) {
      results.at(conepos) -= removedpt[conepos];
    }
    if(m_debug) {
      for(unsigned int conepos=0; conepos<conesizes.size(); ++conepos) {
	msg() << MSG::DEBUG << "Scalar pT sum of tracks around this muon cone " << conesizes.at(conepos) << " after self removal = " << results.at(conepos) << endmsg;
      }
    }
    
  }//m_removeSelf    
  
  return StatusCode::SUCCESS;
}



/** DEPRECATED: Old style isolation, only applicable to EF Muons
 * Sum the track pT in cones around the muon.
 *
 * @param efmuon - EF muon track
 * @param idtrks - Container of id trks
 * @param conesizes - Vector of conesizes (in dR)
 * @param results - Vector where the results for each cone will be stored
 * @param dzvals - Vector where dz vals of good tracks will be stored
 * @param drvals - Vector where dr vals of good tracks will be stored
 */
StatusCode TrigMuonEFTrackIsolationTool::calcTrackIsolation(const TrigMuonEFInfoTrack* efmuon, const Rec::TrackParticleContainer* idtrks,  vector<double> conesizes, vector<double>& results, vector<double>* dzvals, vector<double>* drvals) {

  // create vector for cone sizes

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
      msg() << MSG::DEBUG << "EF muon has combined muon" << endmsg;
    }
    const TrigMuonEFCbTrack* cbmuon = efmuon->CombinedTrack();
    muon = efmuon->CombinedTrack();
    if( cbmuon->getIDTrackParticle() ) {
      muon_idtrk = cbmuon->getIDTrackParticle();
      muidtrk_perigee = muon_idtrk->measuredPerigee();
      selfpt = muon_idtrk->pt();
      if(m_debug) {
	msg() << MSG::DEBUG << "Found ID track attached to combined muon, pt = " << selfpt << endmsg;
      }
    }
    else {
      msg() << MSG::WARNING << "Found EF combined muon without a link to ID track, will use combined pt for self removal" << endmsg;
      selfpt = cbmuon->pt();
    }
  }//combinedmuon
  else {
    // for extrapolated muons use the extrapolated muon for self removal
    if(!efmuon->hasExtrapolatedTrack()) {
      if(m_removeSelf) {
	msg() << MSG::WARNING << "This EF muon has neither a combined or extrapolated muon and removeSelf is requested, do not process further" << endmsg;
	return StatusCode::FAILURE;
      }//m_removeSelf
      else { //use spectrometer track
	
	if(!efmuon->hasSpectrometerTrack()) {
	  msg() << MSG::WARNING << "This EF muon has neither a combined, extrapolated or MS muon, do not process further" << endmsg;
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
    msg() << MSG::WARNING << "Inconsistency, removeSelf requested, but selfpt = 0" << endmsg;
  }
  if(!muon) {
    msg() << MSG::WARNING << "Could not find a muon to update with the isolation, skipping this muon" << endmsg;
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
	    msg() << MSG::DEBUG << "ID track failed dz cut, ignoring it. dz = " << dz << endmsg;
	  }
	  continue;
	}//failed delta(z)
	// store dz (after cut)
	if(dzvals) dzvals->push_back(dz);
	if(m_debug) {
	  msg() << MSG::DEBUG << "ID track passes dz cut. dz = " << dz << endmsg;
	}
      } else {
	msg() << MSG::WARNING << "Can not make delta(z) cut because id trk has invalid measuredPerigee. This track will be included in the isolation calculation" << endmsg;
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
	  msg() << MSG::DEBUG << "ID track within annulus, ignoring it, dR = " << dr << ", annulus = " << m_annulusSize << endmsg;
	 
	}
	continue; // skip tracks in the annulus
      }//annulus cut
      if(m_debug) {
	msg() << MSG::DEBUG << "ID track outside annulus, keep it, dR = " << dr << ", annulus = " << m_annulusSize << endmsg;
      }
    }//use Annulus

    // Define new conesizes that depend on useVarIso flag
    double ptvarcone20 = std::min(0.2,10/selfpt); 
    double ptvarcone30 = std::min(0.3,10/selfpt); 
    if (m_useVarIso){
      conesizes.clear();
      conesizes.push_back(ptvarcone20);
      conesizes.push_back(ptvarcone30);
    }

    // add trk pT to relevant results vector
    for(unsigned int conepos=0; conepos<conesizes.size(); ++conepos) {
      if(dr < conesizes.at(conepos) ) {
	if(m_debug) {
	  msg() << MSG::DEBUG << "Adding trk pt = " << (*trkit)->pt() << ", with dr = " << dr << ", into cone = " << conesizes.at(conepos) << endmsg; 
	}
	results.at(conepos) += (*trkit)->pt();
      }
    }
    
  }//ID track loop
  
  if(m_debug) {
    for(unsigned int conepos=0; conepos<conesizes.size(); ++conepos) {
      msg() << MSG::DEBUG << "Scalar pT sum of tracks around this muon cone " << conesizes.at(conepos) << " = " << results.at(conepos) << endmsg;
    }
  }
  if(m_removeSelf && !m_useAnnulus) { // remove muon pt from the sums
    for(unsigned int conepos=0; conepos<conesizes.size(); ++conepos) {
      results.at(conepos) -= selfpt;
    }
    if(m_debug) {
      for(unsigned int conepos=0; conepos<conesizes.size(); ++conepos) {
	msg() << MSG::DEBUG << "Scalar pT sum of tracks around this muon cone " << conesizes.at(conepos) << " after self removal = " << results.at(conepos) << endmsg;
      }
    }
    
  }//m_removeSelf    
  
  return StatusCode::SUCCESS;
}

StatusCode TrigMuonEFTrackIsolationTool::initialize() {

  m_debug = msgLvl(MSG::DEBUG);

  if(m_debug) {
    msg() << MSG::DEBUG << "Initializing TrigMuonEFTrackIsolationTool[" << name() << "]" << endmsg;
    msg() << MSG::DEBUG
	  << "package version = " << PACKAGE_VERSION << endmsg;
    msg() << MSG::DEBUG
	  << "Properties set as follows: " << endmsg;
    msg() << MSG::DEBUG
	  << "removeSelf                     " << m_removeSelf << endmsg;
    msg() << MSG::DEBUG
	  << "deltaZCut                      " << m_deltaz_cut << endmsg;
    msg() << MSG::DEBUG
	  << "useAnnulus                     " << m_useAnnulus << endmsg;
    msg() << MSG::DEBUG
	  << "annulusSize                    " << m_annulusSize << endmsg;
    msg() << MSG::DEBUG
	  << "useVarIso                    " << m_useVarIso << endmsg;

  }//debug

  // check the annulus size is valid
  if(m_useAnnulus) {
    if(m_annulusSize < 0.0 || m_annulusSize > 0.2) {
      msg() << MSG::ERROR << "Bad configuration of annulusSize = " << m_annulusSize << ", fix your config!" << endmsg;
      return StatusCode::FAILURE;
    }
  }//useAnnulus

  return StatusCode::SUCCESS;
}

StatusCode TrigMuonEFTrackIsolationTool::finalize() {

  return StatusCode::SUCCESS;
}
