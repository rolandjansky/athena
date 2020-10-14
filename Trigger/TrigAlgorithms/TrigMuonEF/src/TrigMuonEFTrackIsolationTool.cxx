/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// local includes
#include "TrigMuonEFTrackIsolationTool.h"

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
  m_trkSelTool( "InDet::InDetTrackSelectionTool/TrackSelectionTool", this )
{  
  
  declareInterface<IMuonEFTrackIsolationTool>(this);

  declareProperty("deltaZCut", m_deltaz_cut);
  declareProperty("removeSelf", m_removeSelf);
  declareProperty("useAnnulus", m_useAnnulus);
  declareProperty("annulusSize", m_annulusSize);
  declareProperty("useVarIso", m_useVarIso);
  declareProperty("TrackSelectionTool",m_trkSelTool);
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
 * @param selfremoval - Vector storing the selfpt and removed pt 
 */

StatusCode TrigMuonEFTrackIsolationTool::calcTrackIsolation(const xAOD::Muon* efmuon, const xAOD::TrackParticleContainer* idtrks, vector<double> conesizes, vector<double>& results, vector<double>* dzvals, vector<double>* drvals, vector<double>* selfremoval) const {


  if (m_debug)
    msg() << MSG::DEBUG << "Running isolation over EF Muon!" << endmsg;
  
  // clear vectors
  results.clear();
  dzvals->clear();
  drvals->clear();
  selfremoval->clear();

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

 StatusCode isoResult = checkIsolation(muon, selfpt, muon_idtrk, muidtrk_perigee, idtrks, conesizes, results, dzvals, drvals, selfremoval);

  return isoResult;
}


// Generic function to take whatever kind of muon and check isolation (so can pass a L2 or EF muon)
StatusCode TrigMuonEFTrackIsolationTool::checkIsolation(const xAOD::IParticle* muon, double selfpt, const xAOD::TrackParticle* muon_idtrk, const Trk::Perigee* muidtrk_perigee, const xAOD::TrackParticleContainer* trks, vector<double> conesizes, vector<double>& results, vector<double>* dzvals, vector<double>* drvals, vector<double>* selfremoval) const {
  
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
  
    
  for(xAOD::TrackParticleContainer::const_iterator trkit=trks->begin();
      trkit!=trks->end(); ++trkit) {

    if (m_debug)

      msg() << MSG::DEBUG << "INFO: Track pT = " << (*trkit)->pt() << " eta = " << (*trkit)->eta() << endmsg;

    // check track passes the selection tool
    const auto& trkSelResult = m_trkSelTool->accept(*trkit);
    if(trkSelResult) {
      ATH_MSG_DEBUG("Track passes selection tool");
    } else {
      if(m_debug) {
	ATH_MSG_DEBUG("Track failed selection tool");
	for(unsigned int i=0; i<trkSelResult.getNCuts (); ++i) {
	  ATH_MSG_DEBUG("   Cut " << i << trkSelResult.getCutName(i).data() << " pass = " << trkSelResult.getCutResult(i));
	}
      }
      continue; // skip this track
    }
    
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

      }
    }
    
  }// track loop

  // Muon pT plots
  selfremoval->push_back(selfpt);
  selfremoval->push_back(muon->pt());

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

  if(m_trkSelTool.retrieve().isFailure()){
    ATH_MSG_FATAL("Could not retrieve InDetTrackSelectionTool");    
    return StatusCode::FAILURE;
  }

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
	  << "useVarIso                      " << m_useVarIso << endmsg;
    msg() << MSG::DEBUG
          << "TrackSelectionTool             " << m_trkSelTool << endmsg;

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
