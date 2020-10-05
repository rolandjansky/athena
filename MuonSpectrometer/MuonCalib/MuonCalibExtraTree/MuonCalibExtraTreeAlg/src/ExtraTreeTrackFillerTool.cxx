/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeAlg/ExtraTreeTrackFillerTool.h"
#include "MuonCalibExtraTreeAlg/ISegmentOnTrackSelector.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"
#include "MuonCalibExtraTreeAlg/MuonCalibTrack_EBranch.h"
#include "MuonCalibExtraTreeAlg/MuonCalibHit_EBranch.h"
#include "MuonCalibExtraTreeAlg/MuonCalibTrackSegmentBranch.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHole_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTrack_E.h"
#include "AthContainers/DataVector.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/Surface.h"

#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h" 

#include "TrkEventPrimitives/ResidualPull.h"
#include "CxxUtils/sincos.h"

#include "EventPrimitives/EventPrimitives.h"

namespace MuonCalib {

ExtraTreeTrackFillerTool::ExtraTreeTrackFillerTool(const std::string &type, const std::string &name, const IInterface *parent) : 
  IExtraTreeFillerTool(), 
  AthAlgTool(type, name, parent),
  m_author(0) {
  declareInterface<IExtraTreeFillerTool>(this);
  declareProperty("TrackCollectionKey", m_trackCollectionKey);
  declareProperty("SegmentAuthors", m_segment_authors);
  declareProperty("TrackAuthor", m_author);
}
	
StatusCode ExtraTreeTrackFillerTool::initialize() {

  ATH_CHECK(m_idHelperSvc.retrieve());

  if(!m_trackCollectionKey.size()) {
    ATH_MSG_FATAL("No TrackCollectionKey set!");
    return StatusCode::FAILURE;
  }
  if(!retrieveTools().isSuccess()) {
    ATH_MSG_FATAL("Retrieve Tools failed!");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode ExtraTreeTrackFillerTool::writeTracks(unsigned int &index) {
  //retrieve SG container
  const TrackCollection* tracks;
  if(!evtStore()->retrieve(tracks, m_trackCollectionKey).isSuccess()) {
    ATH_MSG_FATAL("Cannot retrieve track collection!");
    return StatusCode::FAILURE;
  }
  if(!handleTracks(tracks,  index, m_author)) {
    ATH_MSG_FATAL("Failed to fill tracks!");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

inline bool  ExtraTreeTrackFillerTool::handleTracks(const TrackCollection *tracks, unsigned int &index, const int &author) {
  TrackCollection::const_iterator track = tracks->begin();
  TrackCollection::const_iterator last_track = tracks->end();
  for( ; track!=last_track; ++track ) {
    handleTrack(*track, index, author, true);
  }
  return true;
}

void ExtraTreeTrackFillerTool::handleTrack(const Trk::Track *track, unsigned int &index, int author,  bool storeHits) {	
  if (!storeTrack(track, author, m_segment_authors_set)) return;
  if(storeHits) {
    // Track "States on surfaces" are really the points in the track, points being scatterers, hits, etc
    const DataVector<const Trk::TrackStateOnSurface> *trackStates = (track)->trackStateOnSurfaces();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator its     = trackStates->begin() ;
    DataVector<const Trk::TrackStateOnSurface>::const_iterator its_end = trackStates->end() ;
    ATH_MSG_DEBUG(" MuonCalibExtraTreeAlg track states " << trackStates->size());
    for( ; its != its_end ; ++its ) {
      if( (*its)->type(Trk::TrackStateOnSurface::Scatterer) ) {
	storeScatteringCenter((*its), index);
	continue;
      }
      if ( (*its)->type(Trk::TrackStateOnSurface::Hole) )  {
	storeHole(*its, index);
	continue;
      }
      const Trk::MeasurementBase* measurement = (*its)->measurementOnTrack();
      if (!measurement)
	continue;
      storeMeasurement(measurement, *its, index);
    }
  }
  ++index;
}  //end ExtraTreeTrackFillerTool::handleTrack

inline void ExtraTreeTrackFillerTool::storeMeasurement(const Trk::MeasurementBase *measurement, const Trk::TrackStateOnSurface *tsos, unsigned int &index) {
  int type = -1;
  const Trk::RIO_OnTrack *rot = dynamic_cast<const Trk::RIO_OnTrack*> (measurement) ;
  double error = std::sqrt(measurement->localCovariance()(0,0));
  MuonFixedId id;
  Identifier idc;
  bool ismdt = false;
  if (rot) { 
    type = 1;
    if (m_idHelperSvc->isMuon(rot->identify())) {
      id = m_idToFixedIdTool->idToFixedId(rot->identify());
      if(m_idHelperSvc->isMdt(rot->identify())) ismdt = true;
    }
  } else { 
    const Trk::CompetingRIOsOnTrack *rotc = dynamic_cast<const Trk::CompetingRIOsOnTrack*> (measurement);
    if (rotc) {
      idc = rotc->rioOnTrack(0).identify();
      if (m_idHelperSvc->isMuon(idc)) id = m_idToFixedIdTool->idToFixedId(idc);
      type = 2;
      if(id.is_tgc()) error = errorCompetingRot(rotc);
    } else {
      const Trk::PseudoMeasurementOnTrack *rotp = dynamic_cast<const Trk::PseudoMeasurementOnTrack*> (measurement);
      if (rotp) {
        type = 3;
      }
    }
  }
  Amg::Vector3D pos = measurement->globalPosition();
  const Trk::TrackParameters *trackPars = tsos->trackParameters();
  if (ismdt) {
    // Store for Mdt wire position (crossed by track for local Z) 
    Amg::Vector2D lpos(0., trackPars ? trackPars->parameters()[Trk::locZ] : 0.);
    const Amg::Vector3D* gpos = measurement->associatedSurface().localToGlobal(lpos);
    pos = Amg::Vector3D (gpos->x(),gpos->y(),gpos->z());
    ATH_MSG_DEBUG(" MDT Local position " <<  lpos << " MDT global WIRE position measurement " << pos);
    delete gpos;
  } 
  double driftRadius(0.);
  if( measurement->localParameters().contains(Trk::locX) ){
    driftRadius = measurement->localParameters()[Trk::locX];
  } else if( measurement->localParameters().contains(Trk::locY) ){
    driftRadius = measurement->localParameters()[Trk::locY];
  }

  double residual = -999.;
  double pull = -999.;
  if( trackPars ) {
    const Trk::ResidualPull* resPull = m_pullCalculator->residualPull( measurement, trackPars, Trk::ResidualPull::Unbiased );
    if( resPull ){
      residual = resPull->residual().front();
      pull = resPull->pull().front();
      delete resPull;
    }
    ATH_MSG_DEBUG("residual " << residual << " pull " << pull);
  }
      
// Outliers 
  if ( tsos->type(Trk::TrackStateOnSurface::Outlier) ) {
    ATH_MSG_DEBUG(" outlier ");
    if (!trackPars) return;
    const Muon::MdtDriftCircleOnTrack* mdtROT = dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(measurement);
    if(mdtROT) {
      double radialResidual = std::abs(mdtROT->localParameters()[Trk::locR])-std::abs(trackPars->parameters()[Trk::locR]);
      if( radialResidual < 0. ) {
	type = 4 ;
	ATH_MSG_DEBUG(" out of time  ");
      } else {
	type = 5 ;
	ATH_MSG_DEBUG(" delta  ");
      }
    }
  }   
  
  // Get drift time for MDT hits
  double driftTime(0.);
  if( ismdt ) {
    const Muon::MdtDriftCircleOnTrack *mdt_DCOT = dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(measurement);
    driftTime = mdt_DCOT->driftTime();
  }

  const MuonCalibHit_E muonHit(id, pos, driftRadius, error, residual, pull, type);
  //  Add driftTime to track hits.  Could have added driftTime to MuonCalibHit_E class but this is 
  //  widely used, and this is the only place where driftTime is needed. (E. Diehl)
  m_hitBranch->fillBranch( muonHit, driftTime, index );
  if (type == 2 && (id.is_rpc() || id.is_tgc()) ) {
    const Trk::CompetingRIOsOnTrack *rotc = dynamic_cast<const Trk::CompetingRIOsOnTrack*> (measurement);
    ATH_MSG_DEBUG(" Competing rio " << type << " error " << error);
    if( rotc->numberOfContainedROTs() > 1) {       
      for (unsigned int i=1; i<rotc->numberOfContainedROTs(); i++) {
	rot = &(rotc->rioOnTrack(i));
	if(!rot) {
	  ATH_MSG_WARNING("ROI on Track not available - skipping this one.");
	  continue;
	}
	idc = rot->identify(); 
	id = m_idToFixedIdTool->idToFixedId(idc);
	residual = -999.;
	pull = -999.;
	Amg::Vector3D pos = rot->globalPosition();
	error = std::sqrt(rot->localCovariance()(0,0));
	if(id.is_tgc() && rot) {
	  error = errorRot(rot);
	}
	ATH_MSG_DEBUG(" Competing rio " << -type << " error " << error);
	const MuonCalibHit_E muonHit(id, pos, driftRadius, error, residual, pull, -type);
	m_hitBranch->fillBranch( muonHit, driftTime, index );
      }
    }
  }
}  //end ExtraTreeTrackFillerTool::storeMeasurement
        
StatusCode ExtraTreeTrackFillerTool::retrieveTools() {
  if(!m_idToFixedIdTool.retrieve().isSuccess())	{
    ATH_MSG_FATAL("Failed to retrieve IdToFixedIdTool!");
    return StatusCode::FAILURE;
  }
  if(!m_pullCalculator.retrieve().isSuccess()) {
    ATH_MSG_FATAL("Failed to reteive pullCalculator!");
    return StatusCode::FAILURE;
  }

//initialize segment author set if not done
  if(m_segment_authors.size() && !m_segment_authors_set.size()) {
    for(std::vector<int>::const_iterator it=m_segment_authors.begin(); it!=m_segment_authors.end(); it++) {
      m_segment_authors_set.insert(*it);
    }
  }
  return StatusCode::SUCCESS;
}  // end ExtraTreeTrackFillerTool::retrieveTools

inline bool ExtraTreeTrackFillerTool::storeTrack(const Trk::Track *track, int author, const std::set<int> &seg_authors) {
  const Trk::Perigee* perigee = track->perigeeParameters();
  ATH_MSG_DEBUG(" storeTrack  ");
  if( !perigee ) {
    ATH_MSG_WARNING(" NO Perigee  ");
    return false;
  }
  bool succes = true;
  const Trk::Surface* persurf = &(perigee->associatedSurface());
  if(persurf) { 
    if (author == 0 && std::abs(persurf->center().perp()) < 10.) {
      //    Look for Muon Entry
      succes = false;  
      const DataVector<const Trk::TrackStateOnSurface>* trackStates = track->trackStateOnSurfaces();
      if( trackStates ) { 
	DataVector<const Trk::TrackStateOnSurface>::const_iterator its     = trackStates->begin() ;
	DataVector<const Trk::TrackStateOnSurface>::const_iterator its_end = trackStates->end() ;
	ATH_MSG_DEBUG(" MuonCalibExtraTreeAlg track states " << trackStates->size() << " author " << author);
	for( ; its != its_end ; ++its ) {
	  const Trk::TrackParameters* pars = (*its)->trackParameters();
	  if(!pars) continue; 
	  persurf = &(pars->associatedSurface());
	  if(!persurf) continue; 
	  ATH_MSG_DEBUG(" Muon perigee surface found at perp " <<  persurf->center().perp());
	  if (persurf->center().perp()<10) continue;
	  const Trk::Perigee* pp = dynamic_cast<const Trk::Perigee*>(pars);
	  if(!pp) continue;
	  perigee = pp;
	  ATH_MSG_DEBUG(" Muon Entry perigee found at perp " << persurf->center().perp() << " z " << persurf->center().z());
	  succes = true;
	  break; 
	}
      }// end trackStates
    }// end perp cut  
  }
  if(!succes) perigee = track->perigeeParameters();
  if(!perigee) return false;
//      if (!succes) return false;   
  Amg::Vector3D pos = perigee->position();
  Amg::Vector3D direction = perigee->momentum();
//      double charge = perigee->charge();
  double z0 = perigee->parameters()[Trk::z0] ;
  double d0 = perigee->parameters()[Trk::d0] ;
  double phi = perigee->parameters()[Trk::phi0] ;
  double theta = perigee->parameters()[Trk::theta] ;
  double qOverP = perigee->parameters()[Trk::qOverP] ;
  double chi2 = -999.;
  int    ndof = -999;
  if( (track)->fitQuality() ){
    chi2 = (track)->fitQuality()->chiSquared() ;
    ndof = (track)->fitQuality()->numberDoF() ;
  }
  //double errP = -999.;
  double cov00 = -999.;
  double cov01 = -999.;
  double cov02 = -999.;
  double cov03 = -999.;
  double cov04 = -999.;
  double cov11 = -999.;
  double cov12 = -999.;
  double cov13 = -999.;
  double cov14 = -999.;
  double cov22 = -999.;
  double cov23 = -999.;
  double cov24 = -999.;
  double cov33 = -999.;
  double cov34 = -999.;
  double cov44 = -999.;
  const Trk::Perigee* mpp = dynamic_cast<const Trk::Perigee*>(perigee);
  if( mpp && mpp->covariance()) {
    cov00 = (*(mpp->covariance()))(0,0);
    cov01 = (*(mpp->covariance()))(0,1);
    cov02 = (*(mpp->covariance()))(0,2);
    cov03 = (*(mpp->covariance()))(0,3);
    cov04 = (*(mpp->covariance()))(0,4);
    cov11 = (*(mpp->covariance()))(1,1);
    cov12 = (*(mpp->covariance()))(1,2);
    cov13 = (*(mpp->covariance()))(1,3);
    cov14 = (*(mpp->covariance()))(1,4);
    cov22 = (*(mpp->covariance()))(2,2);
    cov23 = (*(mpp->covariance()))(2,3);
    cov24 = (*(mpp->covariance()))(2,4);
    cov33 = (*(mpp->covariance()))(3,3);
    cov34 = (*(mpp->covariance()))(3,4);
    cov44 = (*(mpp->covariance()))(4,4);
  }
  ATH_MSG_DEBUG(" Track position " << pos << " d0 " << d0 << " z0 " << z0 
		<< " x " << pos.x() << " y " << pos.y() << " z " << pos.z() 
		<< " theta Per " << theta << " theta " << direction.theta() 
		<<   " phi Per " << phi << " phi " << direction.phi()); 
  double p = 0.;
  if(qOverP!=0) p = 0.001/(qOverP); 
  ATH_MSG_DEBUG(" Track charge * mom perigee " << p); 

  const MuonCalibTrack_E muonTrack(pos.x(), pos.y(), pos.z(), phi, theta, qOverP, author, cov00, cov01, cov02, cov03, cov04, cov11, cov12, cov13, cov14, cov22, cov23, cov24, cov33, cov34, cov44, chi2, ndof);
  m_trackBranch->fillBranch( muonTrack );
// Segment on track search 
  ATH_MSG_DEBUG(" trkSeg: seg_authors.size()=" << seg_authors.size());
  if(m_segmentOnTrackSelector) ATH_MSG_DEBUG( "m_segmentOnTrackSelector is empty? " << m_segmentOnTrackSelector->empty() ); 
  if(seg_authors.size() && m_segmentOnTrackSelector && !m_segmentOnTrackSelector->empty()) {
    m_trackSegmentBranch->fillBranch(
      (*m_segmentOnTrackSelector)->SegmentIndices(*track, seg_authors), m_trackBranch->getBranchEntries()-1
    );
  }

  return true;
} //end ExtraTreeTrackFillerTool::storeTrack
	
inline double ExtraTreeTrackFillerTool::errorCompetingRot( const Trk::CompetingRIOsOnTrack *rotc ) {
//  For the TGCs diagonalize the error matrix and use the smallest component
  if(!rotc) return 0.;
  double error = rotc->localCovariance()(0,0);
  if( rotc->localCovariance().cols() > 1 ){
    AmgMatrix(2,2) Er; Er.setZero();
    Er(0, 0) = rotc->localCovariance()(0,0);
    Er(0, 1) = rotc->localCovariance()(0,1);
    Er(1, 1) = rotc->localCovariance()(1,1);
    Er(1, 0) = Er(0,1);   
      
    double chi = Er(0,0) != Er(1,1) ? std::atan(-2*Er(0,1)/(Er(0,0)-Er(1,1)))/2. : 0.;
 
    double sincoschi[2];
    CxxUtils::sincos scchi(chi);
    sincoschi[0] = scchi.sn;
    sincoschi[1] = scchi.cs;
    AmgMatrix(2,2) Rot; Rot.setZero();
    Rot(0, 0) = sincoschi[1];
    Rot(1, 1) = Rot(0,0);
    Rot(0, 1) = sincoschi[0];
    Rot(1, 0) = -Rot(0,1);
    AmgMatrix(2,2) D = Rot.transpose()*Er*Rot;
    ATH_MSG_DEBUG(" Diagonalized error matrix " << D << " er " << Er);
    error = D(0, 0) < D(1, 1) ? D(0,0) : D(1, 1);
  }
  return std::sqrt(std::abs(error)); 
}  //end ExtraTreeTrackFillerTool::errorCompetingRot

inline double ExtraTreeTrackFillerTool::errorRot( const Trk::RIO_OnTrack* rot) {
//  For the TGCs diagonalize the error matrix and use the smallest component
  if(!rot) return 0.;
  double error = rot->localCovariance()(1,1);
  if( rot->localCovariance().cols() > 1 ){
    AmgMatrix(2,2) Er; Er.setZero();
    Er(0,0) = rot->localCovariance()(0,0);
    Er(0,1) = rot->localCovariance()(1,2);
    Er(1,1) = rot->localCovariance()(2,2);
    Er(1,0) = Er(0,1);   
      
    double chi = Er(0,0) != Er(1,1) ? std::atan(-2*Er(0,1)/(Er(0,0)-Er(1,1)))/2. : 0.;
 
    double sincoschi[2];
    CxxUtils::sincos scchi(chi);
    sincoschi[0] = scchi.sn;
    sincoschi[1] = scchi.cs;
 
    AmgMatrix(2,2) Rot; Rot.setZero();
    Rot(0,0) = sincoschi[1];
    Rot(1,1) = Rot(0,0);
    Rot(0,1) = sincoschi[0];
    Rot(1,0) = -Rot(0,1);
    AmgMatrix(2,2) D = Rot.transpose()*Er*Rot;
    ATH_MSG_DEBUG(" Diagonalized error matrix " << D << " er " << Er);
    error = D(0,0) < D(1,1) ? D(0,0) : D(1,1);
  }
  return std::sqrt(std::abs(error)); 
}  //end ExtraTreeTrackFillerTool::errorRot
  
inline void ExtraTreeTrackFillerTool::storeHole(const Trk::TrackStateOnSurface *tsos, const unsigned int &index) {
  ATH_MSG_DEBUG(" Hole on track ");
  const Trk::TrackParameters* trackPars = tsos->trackParameters();
  if(!trackPars) return;
  Amg::Vector3D pos = trackPars->position();
  const Identifier idSharka = tsos->trackParameters()->associatedSurface().associatedDetectorElementIdentifier();
  MuonFixedId idfSharka = m_idToFixedIdTool->idToFixedId(idSharka);
  ATH_MSG_DEBUG(" Hole stored at position x " << pos.x() << " y " << pos.y() << " z " <<pos.z());
  int type = 6;
  double tradius = std::abs(trackPars->parameters()[Trk::locR]);
  const MuonCalibHit_E muonHit(idfSharka,pos,tradius,0.,0.,0.,type);
  m_hitBranch->fillBranch( muonHit, 0., index );
}  //end ExtraTreeTrackFillerTool::storeHole(

inline void ExtraTreeTrackFillerTool::storeScatteringCenter(const Trk::TrackStateOnSurface *tsos, const unsigned int &index) {
  ATH_MSG_DEBUG("TSOS is a scattering center ");

  MuonFixedId id;
  const Trk::TrackParameters* pars = tsos->trackParameters();
  Amg::Vector3D pos = pars ? pars->position() : Amg::Vector3D(0.0, 0.0, 0.0);
  //	  double lpos = 0.;
  const Trk::MaterialEffectsBase* matEffBase  = tsos->materialEffectsOnTrack();
  const Trk::MaterialEffectsOnTrack* matEff = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(matEffBase);

  double x0 = -999.;
  //	  double dp = -999.;
  //	  double sigQP2 = -999.;
  double dphi = -999.;
  double sigDPhi = -999.;
  double pullDPhi = -999.;
	  
  double dtheta = -999.;
  double sigDTheta = -999.;
  double pullDTheta = -999.;
  
  if( matEff ) {
    x0 = matEff->thicknessInX0();
    ATH_MSG_DEBUG(" found MaterialEffectsOnTrack:  x0 " << x0); 
    const Trk::ScatteringAngles* scatAng = (matEff)->scatteringAngles();
    if( scatAng ) {
      dphi = scatAng->deltaPhi();	  
      sigDPhi = scatAng->sigmaDeltaPhi();	  
      pullDPhi = dphi/sigDPhi;
      
      dtheta = scatAng->deltaTheta();	  
      sigDTheta = scatAng->sigmaDeltaTheta();	  
      pullDTheta = dtheta/sigDTheta;
      
      ATH_MSG_DEBUG(" found ScatteringAngleOnTrack: dphi " << dphi << " sig " << sigDPhi 
		    << " dtheta " << dtheta << " sig " << sigDTheta);
    }
  }
  const MuonCalibHit_E muonHitPhi(id, pos, x0, sigDPhi, dphi, pullDPhi, 11);
  const MuonCalibHit_E muonHitEta(id, pos, x0, sigDTheta, dtheta, pullDTheta, 12);
  m_hitBranch->fillBranch( muonHitPhi, 0., index );
  m_hitBranch->fillBranch( muonHitEta, 0., index );
}  //end ExtraTreeTrackFillerTool::storeScatteringCenter

}  //namespace MuonCalib
