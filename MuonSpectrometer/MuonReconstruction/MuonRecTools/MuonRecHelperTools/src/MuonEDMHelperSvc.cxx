/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRecHelperTools/MuonEDMHelperSvc.h"

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

  MuonEDMHelperSvc::MuonEDMHelperSvc(const std::string& name, ISvcLocator* svc) : base_class(name, svc) {}
  StatusCode MuonEDMHelperSvc::initialize() {
    ATH_CHECK(AthService::initialize());
    ATH_CHECK(m_idHelperSvc.retrieve());
    return StatusCode::SUCCESS;
  }

  Identifier MuonEDMHelperSvc::getIdentifier( const Trk::MeasurementBase& meas ) const {
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


  Identifier MuonEDMHelperSvc::chamberId( const MuonSegment& seg ) const {
    Identifier chid;
    for(const Trk::MeasurementBase* meas : seg.containedMeasurements()){

      // get Identifier
      Identifier id = getIdentifier(*meas);
      if( !id.is_valid() ) continue;

      // create chamber ID
      chid = m_idHelperSvc->chamberId(id);

      // stop at first none trigger hit
      if( !m_idHelperSvc->isTrigger(id) ) break;
    }

    if( !chid.is_valid() ){
      ATH_MSG_WARNING("Got segment without valid identifiers");
    }

    return chid;
  }

  std::set<Identifier> MuonEDMHelperSvc::chamberIds( const MuonSegment& seg ) const {

    std::set<Identifier> chIds;
    Identifier chid;
    Identifier chidTrig;
    for(const Trk::MeasurementBase* meas : seg.containedMeasurements()){

      // get Identifier
      Identifier id = getIdentifier(*meas);
      if( !id.is_valid() ) continue;

      // create chamber ID
      chid = m_idHelperSvc->chamberId(id);

      // stop at first none trigger hit
      if( !m_idHelperSvc->isTrigger(id) ) chIds.insert(chid);
      else chidTrig = chid;      
    }
    if( chIds.empty() ) {
      chIds.insert(chidTrig);
    }

    return chIds;
  }

  bool MuonEDMHelperSvc::isEndcap( const MuonSegment& seg ) const {
    return m_idHelperSvc->isEndcap( chamberId(seg) );
  }

  bool MuonEDMHelperSvc::isEndcap( const Trk::Track& track ) const {

    const DataVector<const Trk::MeasurementBase>* measurements = track.measurementsOnTrack();
    if( !measurements ) return true;

    // loop over measurements in reversed order (should be faster as endcap hits tend to be at the end of the track)
    DataVector<const Trk::MeasurementBase>::const_reverse_iterator rit = measurements->rbegin();
    DataVector<const Trk::MeasurementBase>::const_reverse_iterator rit_end = measurements->rend();
    for( ;rit!=rit_end;++rit ){
      Identifier id = getIdentifier(**rit);
      if( !id.is_valid() ) continue;

      if( m_idHelperSvc->isEndcap(id) ) return true;
    }
    return false;
  }


  const Trk::AtaPlane* MuonEDMHelperSvc::createTrackParameters( const MuonSegment& seg, double momentum, double charge ) const {

    // we need a none zero momentum
    if( momentum == 0. ) {
      ATH_MSG_WARNING(" cannot create parameters with zero momentum ");
      return 0;
    }
    double locx = seg.localParameters().contains(Trk::locX) ? seg.localParameters()[Trk::locX] : 0.;
    double locy = seg.localParameters().contains(Trk::locY) ? seg.localParameters()[Trk::locY] : 0.;
    double qoverp = charge/momentum;
    return dynamic_cast<const Trk::AtaPlane*>(
      seg.associatedSurface()
        .createUniqueParameters<5, Trk::Charged>(locx,
                                           locy,
                                           seg.globalDirection().phi(),
                                           seg.globalDirection().theta(),
                                           qoverp)
        .release());
  }

  bool MuonEDMHelperSvc::goodTrack( const Trk::Track& track, double chi2Cut ) const {

    // get reduced chi2
    const Trk::FitQuality* fq = track.fitQuality();
    if( !fq || fq->numberDoF() == 0 ){
      return false;
    }

    double reducedChi2 = fq->chiSquared()/fq->numberDoF();
    // reject fit if larger than cut
    return reducedChi2 <= chi2Cut;
  }

  bool MuonEDMHelperSvc::isSLTrack( const Trk::Track& track ) const {
    // use track info if set properly
    if( track.info().trackProperties(Trk::TrackInfo::StraightTrack) ) return true;

    // else try using error matrix
    const Trk::Perigee* pp = track.perigeeParameters();
    if (!pp || !pp->covariance()) return false;   
	  /// sum covariance terms of momentum, use it to determine whether fit was SL fit
	  double momCov = 0.;
	  for( int i=0;i<4;++i ) momCov += std::abs( (*pp->covariance())(4,i) );
    return momCov < 1.e-10;
  }
} //end of namespace
