/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "TrkTrack/AlignmentEffectsOnTrack.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkExInterfaces/IExtrapolator.h"

#include "TrkTrack/Track.h"

namespace Muon {

  MuonEDMHelperTool::MuonEDMHelperTool(const std::string& t, const std::string& n, const IInterface* p) 
    : AthAlgTool(t,n,p),
      m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
      m_extrapolator("Trk::Extrapolator/MuonExtrapolator")  
  {
    declareInterface<MuonEDMHelperTool>(this);
    declareProperty( "MuonExtrapolator",    m_extrapolator);
    declareProperty( "MuonIdHelperTool",    m_idHelper);
  }
    
  MuonEDMHelperTool::~MuonEDMHelperTool() {
    
  }
    
  StatusCode MuonEDMHelperTool::initialize() {

    if( AthAlgTool::initialize().isFailure() ) return StatusCode::FAILURE;
    
    if( !m_idHelper.empty() && m_idHelper.retrieve().isFailure()){
      ATH_MSG_ERROR("Could not get " << m_idHelper);
      return StatusCode::FAILURE;
    }

    if( !m_extrapolator.empty() && m_extrapolator.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not get " << m_extrapolator); 
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  StatusCode MuonEDMHelperTool::finalize() {

    if (AthAlgTool::finalize().isFailure()) return StatusCode::RECOVERABLE;

    return StatusCode::SUCCESS;
  }


  Identifier MuonEDMHelperTool::getIdentifier( const Trk::MeasurementBase& meas ) const {
    const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(&meas);
    if( rot ) return rot->identify();
    const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(&meas);
    if( crot ) {
      if( crot->containedROTs().empty() ){

	ATH_MSG_WARNING(" CompetingMuonClustersOnTrack without contained ROTs ");
	return Identifier();
      }
      if( !crot->containedROTs().front() ) {
	ATH_MSG_WARNING(" CompetingMuonClustersOnTrack contains a ROT pointer that is zero ");
	return Identifier();
      }
      return crot->containedROTs().front()->identify();
    }
    return Identifier();
  }


  Identifier MuonEDMHelperTool::chamberId( const MuonSegment& seg ) const {
    Identifier chid;
    std::vector<const Trk::MeasurementBase*>::const_iterator mit = seg.containedMeasurements().begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator mit_end = seg.containedMeasurements().end();
    for( ;mit!=mit_end;++mit ){

      // get Identifier
      Identifier id = getIdentifier(**mit);
      if( !id.is_valid() ) continue;

      // create chamber ID
      chid = m_idHelper->chamberId(id);
      
      // stop at first none trigger hit
      if( !m_idHelper->isTrigger(id) ) break;
    }
    
    if( !chid.is_valid() ){
      ATH_MSG_WARNING("Got segment without valide identifiers");
    }

    return chid;
  }

  std::set<Identifier> MuonEDMHelperTool::chamberIds( const MuonSegment& seg ) const {

    std::set<Identifier> chIds;
    Identifier chid;
    Identifier chidTrig;
    std::vector<const Trk::MeasurementBase*>::const_iterator mit = seg.containedMeasurements().begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator mit_end = seg.containedMeasurements().end();
    for( ;mit!=mit_end;++mit ){

      // get Identifier
      Identifier id = getIdentifier(**mit);
      if( !id.is_valid() ) continue;

      // create chamber ID
      chid = m_idHelper->chamberId(id);
      
      // stop at first none trigger hit
      if( !m_idHelper->isTrigger(id) ){
	chIds.insert(chid);
      }else{
	chidTrig = chid;
      }
    }
    if( chIds.empty() ) {
      chIds.insert(chidTrig);
    }
    
    return chIds;
  }

  bool MuonEDMHelperTool::isEndcap( const MuonSegment& seg ) const {
    return m_idHelper->isEndcap( chamberId(seg) );
  }

  bool MuonEDMHelperTool::isEndcap( const Trk::Track& track ) const {
    
    const DataVector<const Trk::MeasurementBase>* measurements = track.measurementsOnTrack();
    if( !measurements ) return true;

    // loop over measurements in reversed order (should be faster as endcap hits tend to be at the end of the track)
    DataVector<const Trk::MeasurementBase>::const_reverse_iterator rit = measurements->rbegin();
    DataVector<const Trk::MeasurementBase>::const_reverse_iterator rit_end = measurements->rend();
    for( ;rit!=rit_end;++rit ){
      Identifier id = getIdentifier(**rit);
      if( !id.is_valid() ) continue;
      
      if( m_idHelper->isEndcap(id) ) return true;
    }
    return false;
  }


  const Trk::AtaPlane* MuonEDMHelperTool::createTrackParameters( const MuonSegment& seg, double momentum, double charge ) const {

    // we need a none zero momentum
    if( momentum == 0. ) {
      ATH_MSG_WARNING(" cannot create parameters with zero momentum ");
      return 0;
    }
    double locx = seg.localParameters().contains(Trk::locX) ? seg.localParameters()[Trk::locX] : 0.;
    double locy = seg.localParameters().contains(Trk::locY) ? seg.localParameters()[Trk::locY] : 0.;
    double qoverp = charge/momentum;
    return dynamic_cast<const Trk::AtaPlane*>(seg.associatedSurface().createParameters<5,Trk::Charged>(locx,locy,seg.globalDirection().phi(),seg.globalDirection().theta(),qoverp));
  }


  bool MuonEDMHelperTool::goodTrack( const Trk::Track& track, double chi2Cut ) const {

    // get reduced chi2
    const Trk::FitQuality* fq = track.fitQuality();
    if( !fq || fq->numberDoF() == 0 ){
      return false;
    }
    
    double reducedChi2 = fq->chiSquared()/fq->numberDoF();	
    // reject fit if larger than cut
    if( reducedChi2 > chi2Cut ) {
      return false;
    }
    return true;
  }

  bool MuonEDMHelperTool::isSLTrack( const Trk::Track& track ) {

    // use track info if set properly
    if( track.info().trackProperties(Trk::TrackInfo::StraightTrack) ) return true;

    // else try using error matrix
    const Trk::Perigee* pp = track.perigeeParameters();
    if( pp ){
      if( pp->covariance() ){
	// sum covariance terms of momentum, use it to determine whether fit was SL fit
	double momCov = 0.;
	for( int i=0;i<4;++i ) momCov += fabs( (*pp->covariance())(4,i) );
	for( int i=0;i<4;++i ) momCov += fabs( (*pp->covariance())(i,4) );
	if( momCov < 1e-10 ) {
	  return true;
	}
      }
    }
    return false;
  }

  const Trk::Perigee* MuonEDMHelperTool::createPerigee( const Trk::TrackParameters& pars ) const {
    
    if( m_extrapolator.empty() ) return 0;

    Trk::PerigeeSurface persurf(pars.position());
    
    const Trk::TrackParameters* exPars = m_extrapolator->extrapolateDirectly(pars,persurf);
    
    const Trk::Perigee* pp = dynamic_cast<const Trk::Perigee*>(exPars);
    if( !pp ) {
      
      ATH_MSG_WARNING(" Extrapolation to Perigee surface did not return a perigee!! ");
      delete exPars;
      return 0;
    }
    return pp;
  }

} //end of namespace
