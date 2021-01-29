/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEFInfoCnvTestAlg.h"

#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticle.h"

#include "FourMomUtils/P4Helpers.h"

#include <vector>
#include <iomanip>

static const double ptcheckthres=0.001;
static const double etaphicheckthres=0.0001;

TrigMuonEFInfoCnvTestAlg::TrigMuonEFInfoCnvTestAlg( const std::string& name, ISvcLocator* svcLoc ) :
  AthAlgorithm(name, svcLoc)
{
  declareProperty( "AODContainerName", m_aodContainerName = "HLT_MuonEFInfo" );
  declareProperty( "xAODMuonContainerName", m_xaodMuonContainerName = "HLT_Muons" );
}

StatusCode TrigMuonEFInfoCnvTestAlg::initialize() {
    
    ATH_MSG_INFO( "AODContainerName  = " << m_aodContainerName << ",  xAODMuonContainerName = " << m_xaodMuonContainerName );
    
    // Return gracefully:
    return StatusCode::SUCCESS;
}

StatusCode TrigMuonEFInfoCnvTestAlg::execute() {

  // Retrieve the TrigMuonEFInfoContainer
  const TrigMuonEFInfoContainer* infocont = evtStore()->tryConstRetrieve<TrigMuonEFInfoContainer>(m_aodContainerName);
  if (!infocont) {
    ATH_MSG_ERROR("No TrigMuonEFInfoContainer with key " << m_aodContainerName << " found.");
    return StatusCode::RECOVERABLE;
  }
  ATH_MSG_DEBUG("Retrieved " << m_aodContainerName);
  
  // Retrieve the xAOD::MuonContainer
  const xAOD::MuonContainer* muoncont = evtStore()->tryConstRetrieve<xAOD::MuonContainer>(m_xaodMuonContainerName);
  if(!muoncont) {
    ATH_MSG_ERROR("No xAOD::MuonContainer with key " << m_xaodMuonContainerName << " found.");
    return StatusCode::RECOVERABLE;
  }
  ATH_MSG_DEBUG("Retrieved " << m_xaodMuonContainerName);
		  
  // put all TrigMuonEF tracks into a single vector
  std::vector<const TrigMuonEFInfoTrack*> efinfotracks;
  for(auto infoit : *infocont) {
    const TrigMuonEFInfoTrackContainer* trkcont = infoit->TrackContainer();
    for(auto trk : *trkcont) {

      if(trk->hasCombinedTrack() || trk->hasExtrapolatedTrack()) {
	// for now ignore MS only tracks
	efinfotracks.push_back(trk);
      }

    }//loop on Info track objects
  }//loop on TrigMuonEFInfo objects

  ATH_MSG_DEBUG("N(TrigMuonEFInfoTrack) = " << efinfotracks.size());
  ATH_MSG_DEBUG("N(muons) = " << muoncont->size());

  if(efinfotracks.size() != muoncont->size()) {
    ATH_MSG_ERROR("Inconsistency in number of muons from " << m_xaodMuonContainerName << " and " << m_aodContainerName);
    ATH_MSG_ERROR("Converter code is likely faulty");
    return StatusCode::RECOVERABLE;
  }
  ATH_MSG_DEBUG("Size of old & new vectors matches ok");

  // same number of muons, now loop over them
  for( unsigned int i=0; i<muoncont->size(); ++i) {

    const TrigMuonEFInfoTrack* infotrk = efinfotracks[i];
    const xAOD::Muon* muon = (*muoncont)[i];

    if(!infotrk) {
      ATH_MSG_ERROR("Zero pointer for TrigMuonEFInfoTrack");
      return StatusCode::RECOVERABLE;
    }
    if(!muon) {
      ATH_MSG_ERROR("Zero pointer for xAOD::Muon");
      return StatusCode::RECOVERABLE;
    }
    
    ATH_MSG_DEBUG("Check muon " << i);
    
    ATH_CHECK(compareExtrapolatedTracks(infotrk, muon));
    ATH_CHECK(compareCombinedTracks(infotrk, muon));
    ATH_CHECK(comparePrimaryTracks(infotrk, muon));

  }//loop on muons

  return StatusCode::SUCCESS;
}//execute
 
/**
 * Compare the contents of the old TrigMuonEFInfoTrack and the xAOD::Muon for the track extrapolated
 * from the MS.
 */ 
StatusCode TrigMuonEFInfoCnvTestAlg::compareExtrapolatedTracks(const TrigMuonEFInfoTrack* infotrk, const xAOD::Muon* muon) {

  if( !infotrk->hasExtrapolatedTrack() ) {
    ATH_MSG_DEBUG("TrigMuonEFInfo track has no extrapolated track");
    if(muon->muonSpectrometerTrackParticleLink().isValid() ) {
      ATH_MSG_ERROR("Mismatch: TrigMuonEFInfoTrack has no ME track, xAOD::Muon does");
      return StatusCode::RECOVERABLE;
    }
    
    return StatusCode::SUCCESS; // ok both new and old don't have the track
  }

  if( infotrk->hasExtrapolatedTrack() ) {
    ATH_MSG_DEBUG("TrigMuonEFInfo track has extrapolated track");
    if( !(muon->muonSpectrometerTrackParticleLink().isValid()) ) {
      ATH_MSG_ERROR("Mismatch: TrigMuonEFInfoTrack has ME track, xAOD::Muon does not");
      return StatusCode::RECOVERABLE;
    }
  }

  // now compare the actual tracks
  StatusCode sc = compareTracks( infotrk->ExtrapolatedTrack(), *(muon->muonSpectrometerTrackParticleLink()) );
  if(sc.isFailure()) return sc;
  else ATH_MSG_DEBUG("Passed check on extrapolated track");

  return StatusCode::SUCCESS;
}

/**
 * Compare the contents of the old TrigMuonEFInfoTrack and the xAOD::Muon for the combined track
 */ 
StatusCode TrigMuonEFInfoCnvTestAlg::compareCombinedTracks(const TrigMuonEFInfoTrack* infotrk, const xAOD::Muon* muon) {

  if( !infotrk->hasCombinedTrack() ) {
    ATH_MSG_DEBUG("TrigMuonEFInfo track has no combined track");
    if(muon->combinedTrackParticleLink().isValid() ) {
      ATH_MSG_ERROR("Mismatch: TrigMuonEFInfoTrack has no combined track, xAOD::Muon does");
      return StatusCode::RECOVERABLE;
    }
    
    return StatusCode::SUCCESS; // ok both new and old don't have the track
  }

  if( infotrk->hasCombinedTrack() ) {
    ATH_MSG_DEBUG("TrigMuonEFInfo track has combined track");
    if( !(muon->combinedTrackParticleLink().isValid()) ) {
      ATH_MSG_ERROR("Mismatch: TrigMuonEFInfoTrack has combined track, xAOD::Muon does not");
      return StatusCode::RECOVERABLE;
    }
  }

  // now compare the actual tracks
  StatusCode sc = compareTracks( infotrk->CombinedTrack(), *(muon->combinedTrackParticleLink()) );
  if(sc.isFailure()) return sc;
  else ATH_MSG_DEBUG("Passed check on combined track");

  return StatusCode::SUCCESS;
}

/**
 * In this function we are really checking if we managed to set the correct primary track in the xAOD::Muon
 */ 
StatusCode TrigMuonEFInfoCnvTestAlg::comparePrimaryTracks(const TrigMuonEFInfoTrack* infotrk, const xAOD::Muon* muon) {

  const TrigMuonEFTrack* primtrk=0;
  if( infotrk->hasExtrapolatedTrack() ) {
    primtrk = infotrk->ExtrapolatedTrack();
  }
  if( infotrk->hasCombinedTrack() ) {
    primtrk = infotrk->CombinedTrack();
  }
  if(primtrk!=0 && muon->primaryTrackParticleLink().isValid()) {
    StatusCode sc = compareTracks(primtrk, *(muon->primaryTrackParticleLink()));
    if(sc.isFailure()) return sc;
  } else {
    if(primtrk!=0 || muon->primaryTrackParticleLink().isValid()) {
      ATH_MSG_ERROR("One of the primary tracks is not good, old = " << (primtrk!=0) << ", new = " << muon->primaryTrackParticleLink().isValid() );
      return StatusCode::RECOVERABLE;
    } else return StatusCode::SUCCESS; // this means both are 0, which is ok
  }

  ATH_MSG_DEBUG("Passed check on primary track");

  return StatusCode::SUCCESS;

}//comparePrimaryTracks

StatusCode TrigMuonEFInfoCnvTestAlg::compareTracks(const TrigMuonEFTrack* eftrack, const xAOD::TrackParticle* xaodtrack) {

  if(msgLvl(MSG::VERBOSE)) {
    msg() << MSG::VERBOSE << "Compare Track:" << endmsg;
    msg() << MSG::VERBOSE << "old track pt  = " << eftrack->pt()  << ", new track pt  = " << xaodtrack->pt() << endmsg;
    msg() << MSG::VERBOSE << "old track eta = " << eftrack->eta() << ", new track eta = " << xaodtrack->eta() << endmsg;
    msg() << MSG::VERBOSE << "old track phi = " << eftrack->phi() << ", new track phi = " << xaodtrack->phi() << endmsg;
  }
  const double dpt = std::abs(eftrack->pt() - xaodtrack->pt());
  if( dpt > ptcheckthres*eftrack->pt() ) { // pT check is relative - we accept 0.1% changes
    
    ATH_MSG_ERROR("pT of two tracks is different, old = " << std::fixed << std::setprecision(8) << eftrack->pt() << ", new = " << xaodtrack->pt() << ", dpt = " << dpt);
    return StatusCode::RECOVERABLE;
    
  }
  if( std::abs(eftrack->eta() - xaodtrack->eta()) > etaphicheckthres ) {
    ATH_MSG_ERROR("eta of two tracks is different, old = " << eftrack->eta() << ", new = " << xaodtrack->eta());
    return StatusCode::RECOVERABLE;
  }
  const double dphi = P4Helpers::deltaPhi( eftrack->phi(), xaodtrack->phi() );
  if( std::abs(dphi) > etaphicheckthres) {
    ATH_MSG_ERROR("phi of two tracks is different, old = " << eftrack->phi() << ", new = " << xaodtrack->phi());
    return StatusCode::RECOVERABLE;
  }
   
  return StatusCode::SUCCESS;
}
