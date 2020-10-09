/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ExtraTreeMuonFillerTool.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrkTrack/Track.h"
#include <string>
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkGeometry/MagneticFieldProperties.h"

namespace MuonCalib {

ExtraTreeMuonFillerTool::ExtraTreeMuonFillerTool(const std::string &type, const std::string &name, const IInterface *parent): 
  ExtraTreeTrackFillerTool(type, name, parent),
  m_muonContainer("Muons"),
  m_hitsForSA(true),
  m_hitsForSAE(true),
  m_hitsForCombined(true),
  m_hitsForStatCombined(true) {
  declareProperty("MuonContainer", m_muonContainer);
  declareProperty("AuthorOffset", m_authorOffset);
  declareProperty("HitsForSA", m_hitsForSA);
  declareProperty("HitsForSAE", m_hitsForSAE);
  declareProperty("HitsForCombined", m_hitsForCombined);
  declareProperty("HitsForStatCombined", m_hitsForStatCombined);
}

StatusCode ExtraTreeMuonFillerTool::initialize() {
  if(!retrieveTools().isSuccess()) {
    ATH_MSG_FATAL("Retrieve Tools failed!");
    return StatusCode::FAILURE;
  }
  if(!m_edmHelper.retrieve().isSuccess()) {
    ATH_MSG_FATAL("Failed to retrieve IMuonEDMHelperSvc!");
    return StatusCode::FAILURE;		
  }
  if(!m_propagator.retrieve().isSuccess()) {
    ATH_MSG_FATAL("Failed to retrieve Propagator!");
    return StatusCode::FAILURE;		
  }
  return StatusCode::SUCCESS;	
}  // end ExtraTreeMuonFillerTool::initialize

StatusCode ExtraTreeMuonFillerTool::writeTracks(unsigned int &index) {
  const xAOD::MuonContainer *muonContainer;
  if(evtStore()->retrieve(muonContainer, m_muonContainer).isSuccess()) {
    for(auto muon: *muonContainer) {
      const xAOD::TrackParticle *prim = muon->trackParticle(xAOD::Muon::Primary);
      if(prim) {
	const xAOD::TrackParticle *part = muon->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
	if(part) {
	  if(!writeTrackParticle(part, part==prim, m_hitsForSA, index, 2).isSuccess()){
	    ATH_MSG_FATAL("Failed to write SA track!");
	  }
	}
	part=muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
	if(part) {
	  if(!writeTrackParticle(part, part==prim, m_hitsForSA, index, 1).isSuccess()){
	    ATH_MSG_FATAL("Failed to write ID track!");
	  }
	}
	part=muon->trackParticle(xAOD::Muon::CombinedTrackParticle);
	if(part){
	  if(!writeTrackParticle(part, part==prim, m_hitsForCombined, index, 0).isSuccess()){
	    ATH_MSG_FATAL("Failed to write extrapolated SA track!");
	  }
	}
      }
    }
  }else{
    ATH_MSG_WARNING("Failed to retrieve MuonContainer "<<m_muonContainer);
  }
    
  // CombinedMuonTrackParticles, ExtrapolatedMuonTrackParticles, MuonSpectrometerTrackParticles
  const xAOD::TrackParticleContainer *tracks(0);
  std::string trackSGKey = "MuonSpectrometerTrackParticles";
  if(!evtStore()->retrieve(tracks, trackSGKey).isSuccess()) {
    ATH_MSG_WARNING("Failed to retrieve TrackParticleContainer "<<trackSGKey);
    return StatusCode::SUCCESS;
  }
  for(auto msTrk: *tracks) {
    ATH_MSG_DEBUG("index: " << index << "Muon MS track: pt="<< msTrk->pt() << " eta= " << msTrk->eta());
    if(!writeTrackParticle(msTrk, false, true, index, 6).isSuccess()) {
      ATH_MSG_FATAL("Failed to write SA track!");
      return StatusCode::FAILURE;
    }
  }
    
  const xAOD::TrackParticleContainer *tracksExt(0);
  std::string trackExtSGKey = "ExtrapolatedMuonTrackParticles";
  if(!evtStore()->retrieve(tracksExt, trackExtSGKey).isSuccess()) {
    ATH_MSG_WARNING("Failed to retrieve TrackParticleContainer "<<trackExtSGKey);
    return StatusCode::SUCCESS;
  }
  for(auto msTrk: *tracksExt){
    ATH_MSG_DEBUG("index: " << index << "Muon MS ext. track: pt="<< msTrk->pt() << " eta= " << msTrk->eta());
    if(!writeTrackParticle(msTrk, false, true, index, 7).isSuccess()) {
      ATH_MSG_FATAL("Failed to write extrapolated SAE track!");
      return StatusCode::FAILURE;
    }
  }
  
  const xAOD::TrackParticleContainer *tracksComb(0);
  std::string trackCombSGKey = "CombinedMuonTrackParticles";
  if(!evtStore()->retrieve(tracksComb, trackCombSGKey).isSuccess()) {
    ATH_MSG_WARNING("Failed to retrieve TrackParticleContainer "<<trackCombSGKey);
    return StatusCode::SUCCESS;
  }
  for(auto msTrk: *tracksComb){
    ATH_MSG_DEBUG("index: " << index << "Muon comb. track: pt="<< msTrk->pt() << " eta= " << msTrk->eta());
    if(!writeTrackParticle(msTrk, false, true, index, 8).isSuccess()) {
      ATH_MSG_FATAL("Failed to write combined track!");
      return StatusCode::FAILURE;
    }
  }
  
  return StatusCode::SUCCESS;
}  // end ExtraTreeMuonFillerTool::writeTracks

inline StatusCode ExtraTreeMuonFillerTool::writeTrackParticle(const xAOD::TrackParticle *&part, bool isPrimaryAuthor, bool writeHits, unsigned int &index, int author) {
  const Trk::Track *track=part->track();
  if(!track) {
    ATH_MSG_FATAL("Track is not stored! Are you running on AOD?");
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("going to handle track");
  handleTrack(track, index, author + m_authorOffset + 10*isPrimaryAuthor, writeHits);
  return StatusCode::SUCCESS;
}  //end ExtraTreeMuonFillerTool::writeTrackParticle

inline Trk::Track* ExtraTreeMuonFillerTool::createTaggedMuonTrack( const xAOD::Muon &muon ) const {
  const xAOD::TrackParticle *idtp = muon.trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
  const Trk::Track *track = idtp ? idtp->track() : 0;
  if (!track) return 0;
  ATH_MSG_VERBOSE(" makeSegmentTrack ");
    
  if( !track->trackStateOnSurfaces() || track->trackStateOnSurfaces()->empty() ) return 0;

  Trk::TrackInfo::TrackPatternRecoInfo author       = Trk::TrackInfo::MuGirlUnrefitted;
  if (muon.author() == xAOD::Muon::MuTag) author    = Trk::TrackInfo::StacoLowPt;
  if (muon.author() == xAOD::Muon::MuTagIMO) author = Trk::TrackInfo::StacoLowPt;
  ATH_MSG_VERBOSE(" author " << muon.author());
  DataVector<const Trk::TrackStateOnSurface> *trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>;

  //  Copy ID track
  // if(!m_useMuonHitsOnly){
  DataVector<const Trk::TrackStateOnSurface>::const_iterator it     = track->trackStateOnSurfaces()->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator it_end = track->trackStateOnSurfaces()->end(); 
  for ( ; it!=it_end; ++it) trackStateOnSurfaces->push_back( (*it)->clone() );
  //}

  //  Loop over segments   
  int nseg = muon.nMuonSegments();
  for (int i = 0; i < nseg; ++i) {
//     const Trk::Segment* tseg = muon.muonSegment(i);
//     const Muon::MuonSegment* seg  = dynamic_cast<const  Muon::MuonSegment* > (tseg);
    const xAOD::MuonSegment *segx = muon.muonSegment(i);   
    if (!segx ) {
      ATH_MSG_WARNING("Zero pointer to xAOD::MuonSegment! Skipping.");
      continue;
    }
    const ElementLink < ::Trk::SegmentCollection > &link = segx->muonSegment();
    const Muon::MuonSegment* seg = dynamic_cast<const  Muon::MuonSegment* >(*link);
    if( !seg ) {
      ATH_MSG_WARNING("Invalid link back to Muon::MuonSegment! Skipping.");
      continue;
    }
    // create parameters for muon and loop over hits
    double momentum = 1e8;
    double charge   = 0.;
    const Trk::TrackParameters *pars = m_edmHelper->createTrackParameters( *seg, momentum, charge );
    std::vector<const Trk::MeasurementBase*>::const_iterator mit     = seg->containedMeasurements().begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator mit_end = seg->containedMeasurements().end();
    for( ;mit!=mit_end;++mit ) {
      const Trk::MeasurementBase &meas = **mit;

      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
      typePattern.set(Trk::TrackStateOnSurface::Measurement);

      const Trk::TrackParameters *exPars = m_propagator->propagateParameters(*pars,meas.associatedSurface(),
									     Trk::anyDirection, false, Trk::MagneticFieldProperties());
      if(!exPars){
	ATH_MSG_VERBOSE("Could not propagate Track to segment surface");
      }

      const Trk::TrackStateOnSurface *trackState = new Trk::TrackStateOnSurface( meas.clone(), exPars, 0, 0, typePattern );
      trackStateOnSurfaces->push_back( trackState ); 
    } // end segment loop
    delete pars;
  }
  Trk::TrackInfo info(Trk::TrackInfo::Unknown,Trk::muon);
  info.setPatternRecognitionInfo(author);
  Trk::Track *newtrack = new Trk::Track(info, trackStateOnSurfaces,(track->fitQuality())->clone());
  return newtrack;
}  //end ExtraTreeMuonFillerTool::createTaggedMuonTrack

}//namespace MuonCalib
