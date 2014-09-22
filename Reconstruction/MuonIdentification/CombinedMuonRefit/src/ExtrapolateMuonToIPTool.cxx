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

#include <vector>



ExtrapolateMuonToIPTool::ExtrapolateMuonToIPTool(const std::string& t, const std::string& n,const IInterface* p)  :  
  AthAlgTool(t,n,p),
  m_helper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
  m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
  m_nextrapolations(0),
  m_ngoodExtrapolations(0),
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

  if( AthAlgTool::initialize().isFailure() ){
    ATH_MSG_WARNING( "AthAlgTool::initialize failed" );
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("ExtrapolateMuonToIPTool::initialize()");


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

  if (msgLvl(MSG::DEBUG))  msg(MSG::DEBUG) << " Extrapolated tracks: " << muonTracks.size() << endreq; 

  // loop over muon tracks and extrapolate them to the IP
  TrackCollection::const_iterator tit = muonTracks.begin();
  TrackCollection::const_iterator tit_end = muonTracks.end();
  for( ;tit!=tit_end;++tit ){
    
    Trk::Track* extrapolateTrack = extrapolate(**tit);
    if( !extrapolateTrack ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<MSG::DEBUG <<"Extrapolation of muon to IP failed" << endreq;
      continue;
    }    

    if (msgLvl(MSG::DEBUG))  msg(MSG::DEBUG) << " Extrapolated track " << m_printer->print(*extrapolateTrack) << endreq; 

    extrapolateTracks->push_back(extrapolateTrack);
  }
  return extrapolateTracks;
}

Trk::Track* ExtrapolateMuonToIPTool::extrapolate(const Trk::Track& ) const {
  ATH_MSG_WARNING("This function is no longer supported");
  return 0;
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
    msg(MSG::DEBUG) << " No measured closest parameters found, using none measured parameters" << endreq;
  }
  return closestPars;
}



