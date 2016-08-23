/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ExtrapolateMuonToIPTool.cxx
//   Implementation file for class ExtrapolateMuonToIP
///////////////////////////////////////////////////////////////////
// Niels van Eldik
///////////////////////////////////////////////////////////////////

#include "ExtrapolateMuonToIPTool.h"
#include "TrkParameters/TrackParameters.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "TrkExInterfaces/IExtrapolator.h"

#include <vector>



ExtrapolateMuonToIPTool::ExtrapolateMuonToIPTool(const std::string& t, const std::string& n,const IInterface* p)  :  
  AthAlgTool(t,n,p),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_helper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
  m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
  m_nextrapolations(0),
  //m_ngoodExtrapolations(0),
  m_failedClosestPars(0),
  m_failedExtrapolationLowMom(0),
  m_failedExtrapolationHighMom(0),
  m_failedPerigeeCreation(0)
{
  declareInterface<Muon::IMuonTrackExtrapolationTool>(this);

}

ExtrapolateMuonToIPTool::~ExtrapolateMuonToIPTool() {

} 

// Initialize method:
StatusCode ExtrapolateMuonToIPTool::initialize()
{

  if( m_extrapolator.retrieve().isFailure() ){
    ATH_MSG_ERROR( "failed to retrieve " << m_extrapolator );
    return StatusCode::FAILURE;
  }

  if(!m_helper.empty()) {
    if(m_helper.retrieve().isFailure()) {
      ATH_MSG_ERROR("Failed to retrieve helper " << m_helper);
      return StatusCode::FAILURE;
    }else {
      ATH_MSG_DEBUG( "Retrieved helper " << m_helper );
    }  
  }

  if(!m_printer.empty()) {
    if(m_printer.retrieve().isFailure()) {
      ATH_MSG_ERROR( "Failed to retrieve printer " << m_printer );
      return StatusCode::FAILURE;
    }else {
      ATH_MSG_DEBUG("Retrieved printer " << m_printer);
    }  
  }
  return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode ExtrapolateMuonToIPTool::finalize() 
{
  if( AthAlgTool::finalize().isFailure() ){
    ATH_MSG_WARNING ( "AthAlgTool::finalize failed" );
    return StatusCode::FAILURE;
  }

  double scale = m_nextrapolations != 0 ? 1./m_nextrapolations : 1.;
  ATH_MSG_INFO( "Total number of extrapolations " << m_nextrapolations << " good fraction " << scale*m_nextrapolations << " listing failures" );
  ATH_MSG_INFO( "  no closest parameters           " << scale*m_failedClosestPars );
  ATH_MSG_INFO( "  no extrapolation, low pt        " << scale*m_failedExtrapolationLowMom );
  ATH_MSG_INFO( "  no extrapolation, high pt       " << scale*m_failedExtrapolationHighMom );
  ATH_MSG_INFO( "  problem with perigee creations  " << scale*m_failedPerigeeCreation );

  return StatusCode::SUCCESS;
}


TrackCollection* ExtrapolateMuonToIPTool::extrapolate(const TrackCollection& muonTracks) const {

  TrackCollection* extrapolateTracks = new TrackCollection();
  extrapolateTracks->reserve(muonTracks.size());

  if (msgLvl(MSG::DEBUG))  msg(MSG::DEBUG) << " Extrapolated tracks: " << muonTracks.size() << endmsg; 

  // loop over muon tracks and extrapolate them to the IP
  TrackCollection::const_iterator tit = muonTracks.begin();
  TrackCollection::const_iterator tit_end = muonTracks.end();
  for( ;tit!=tit_end;++tit ){
    
    Trk::Track* extrapolateTrack = extrapolate(**tit);
    if( !extrapolateTrack ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<MSG::DEBUG <<"Extrapolation of muon to IP failed" << endmsg;
      continue;
    }    

    if (msgLvl(MSG::DEBUG))  msg(MSG::DEBUG) << " Extrapolated track " << m_printer->print(*extrapolateTrack) << endmsg; 

    extrapolateTracks->push_back(extrapolateTrack);
  }
  return extrapolateTracks;
}


Trk::Track* ExtrapolateMuonToIPTool::extrapolate(const Trk::Track& track) const {
	
  
  const Trk::TrackInfo& trackInfo = track.info();
  auto particleType = trackInfo.trackProperties(Trk::TrackInfo::StraightTrack) ? Trk::nonInteracting : Trk::muon; 
  const Trk::TrackParameters* closestPars = findMeasuredParametersClosestToIP(track);
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " extrapolating track " << m_printer->print(track) << " type " << particleType << std::endl
                                          << m_printer->printStations(track) << endmsg;
	
  if( !closestPars ){
    msg(MSG::WARNING) <<MSG::WARNING <<"Failed to find closest parameters " <<endmsg;       
    ++m_failedClosestPars;
    return 0;
  }
	
  if (msgLvl(MSG::DEBUG)) {
    // get perigee parameters
    const Trk::Perigee* perigee = track.perigeeParameters();
	
    if( !perigee ){
      msg(MSG::WARNING) << "Muon Track without perigee, skipping " << endmsg;       
    }else{
      msg(MSG::DEBUG) << " closest parameters " << m_printer->print(*closestPars) << endmsg
                      << " perigee            " << m_printer->print(*perigee) << endmsg;
    }
  }
	 
  double dirPosProduct = closestPars->position().dot(closestPars->momentum());
  Trk::PropDirection propDir = dirPosProduct < 0. ? Trk::alongMomentum : Trk::oppositeMomentum;
	 
  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << " scalar product " << dirPosProduct << "  extrapolating ";
    if( propDir == Trk::alongMomentum ) msg(MSG::DEBUG) << " along momentum" << endmsg;
    else                                msg(MSG::DEBUG) << " opposite momentum" << endmsg;
  }
  Trk::PerigeeSurface perigeeSurface(Amg::Vector3D(0.,0.,0.));
  // extrapolate back to IP
  const Trk::TrackParameters* ipPars = m_extrapolator->extrapolate(*closestPars,perigeeSurface,propDir,false);
  if( !ipPars ){
	
    // if extrapolation failed go in other direction
    propDir = (propDir == Trk::alongMomentum) ? Trk::oppositeMomentum : Trk::alongMomentum;
    ipPars = m_extrapolator->extrapolate(*closestPars,perigeeSurface,propDir,false,particleType);
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << " retrying opposite momentum extrapolating ";
      if( propDir == Trk::alongMomentum ) msg(MSG::DEBUG) << " along momentum" << endmsg;
      else                                msg(MSG::DEBUG) << " opposite momentum" << endmsg;
    }
    if( !ipPars){
      if( closestPars->momentum().mag() > 5000. ) ++m_failedExtrapolationLowMom;
      else                                        ++m_failedExtrapolationHighMom;
      return 0;
    }
  }
	
  // create the new track
  // create new perigee
  const Trk::Perigee* ipPerigee = dynamic_cast<const Trk::Perigee*>(ipPars);
  if( !ipPerigee ){
    ipPerigee = m_helper->createPerigee(*ipPars);
    delete ipPars;
  }
	
  if( !ipPerigee ){
    msg(MSG::WARNING) <<"Failed to create perigee for extrapolate track, skipping " <<endmsg;       
    ++m_failedPerigeeCreation;
    return 0;
  }
	
	   
  // create new TSOS DataVector and reserve enough space to fit all old TSOS + one new TSOS
  const DataVector<const Trk::TrackStateOnSurface>* oldTSOT = track.trackStateOnSurfaces();
  DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>();
  unsigned int newSize = oldTSOT->size() + 1;
  trackStateOnSurfaces->reserve( newSize );
	   
  bool perigeeWasInserted = false;
  Amg::Vector3D perDir = ipPerigee->momentum().unit();
	
  // if we didn't start from a parameter in the muon system add perigee to the front
  //trackStateOnSurfaces->push_back( new Trk::TrackStateOnSurface(0,ipPerigee,0,0,Trk::TrackStateOnSurface::Perigee) );
	   
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = oldTSOT->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = oldTSOT->end();
  for( ;tsit!=tsit_end;++tsit ){
	
    // remove old perigee if we didn't start from a parameter in the muon system
    if( (*tsit)->type(Trk::TrackStateOnSurface::Perigee) ) continue;
	   
    const Trk::TrackParameters* pars = (*tsit)->trackParameters();
    if( !pars ) continue;
	
    double distanceOfPerigeeToCurrent = perDir.dot(pars->position() - ipPerigee->position());
	     
    if( !perigeeWasInserted && distanceOfPerigeeToCurrent > 0. ){
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
      typePattern.set(Trk::TrackStateOnSurface::Perigee);
      trackStateOnSurfaces->push_back( new Trk::TrackStateOnSurface(0,ipPerigee,0,0,typePattern) );
      perigeeWasInserted = true;
    }
	
    // copy remainging TSOS
    trackStateOnSurfaces->push_back( (*tsit)->clone() );
  }
	
  if( !perigeeWasInserted ) {
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
    typePattern.set(Trk::TrackStateOnSurface::Perigee);
    trackStateOnSurfaces->push_back( new Trk::TrackStateOnSurface(0,ipPerigee,0,0,typePattern) );
  }
  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << " creating new track " << endmsg;
  }
	
  Trk::TrackInfo info(track.info().trackFitter(),track.info().particleHypothesis());
  info.setPatternRecognitionInfo( Trk::TrackInfo::MuidStandAlone );
  // create new track
  Trk::Track* extrapolateTrack = new Trk::Track( info, trackStateOnSurfaces, track.fitQuality() ? track.fitQuality()->clone():0);
	
  return extrapolateTrack;
}


const Trk::TrackParameters* ExtrapolateMuonToIPTool::findMeasuredParametersClosestToIP( const Trk::Track& track ) const {

  // create new TSOS DataVector and reserve enough space to fit all old TSOS + one new TSOS
  const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
  if( !states ) return 0;

  Trk::PerigeeSurface persurf;
  double rmin = 1e9;
  double rminMeas = 1e9;
  const Trk::TrackParameters* closestPars = 0;
  const Trk::TrackParameters* closestParsMeas = 0;
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
  for( ;tsit!=tsit_end;++tsit ){

    const Trk::TrackParameters* pars = (*tsit)->trackParameters();
    if( !pars ) continue;
    

    double rpars = pars->position().perp();
    if( !closestPars || rpars < rmin ){
      rmin = rpars;
      closestPars = pars;
    }

    if( pars->covariance() && (!closestParsMeas || rpars < rminMeas) ){
      rminMeas = rpars;
      closestParsMeas = pars;
    }
  }
  
  if( closestParsMeas ) {
    return closestParsMeas;
  }else{
    msg(MSG::DEBUG) << " No measured closest parameters found, using none measured parameters" << endmsg;
  }
  return closestPars;
}



