/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackToSegmentTool.h"


#include "TrkParameters/TrackParameters.h"

#include "MuonSegment/MuonSegment.h"

#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonStationIntersectSvc/MuonStationIntersectSvc.h"
#include "MuonStationIntersectSvc/MuonStationIntersect.h"

#include "MuonSegment/MuonSegmentQuality.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/MMClusterOnTrack.h"

#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkEventPrimitives/JacobianPhiThetaLocalAngles.h"

#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkExInterfaces/IPropagator.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include <set>

namespace Muon {

  
  MuonTrackToSegmentTool::MuonTrackToSegmentTool(const std::string& t,const std::string& n,const IInterface* p)  :  
    AthAlgTool(t,n,p),
    m_intersectSvc("MuonStationIntersectSvc", name()),
    m_propagator("Trk::RungeKuttaPropagator/AtlasRungeKuttaPropagator"),
    m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_helperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool")
  {

    declareInterface<IMuonTrackToSegmentTool>(this);

    declareProperty("MuonStationIntersectSvc", m_intersectSvc);
    declareProperty("Propagator",              m_propagator);
    declareProperty("IdHelper",                m_idHelperTool);
    declareProperty("EDMHelper",               m_helperTool);
    declareProperty("EDMPrinter",              m_printer);

  }
    
  MuonTrackToSegmentTool::~MuonTrackToSegmentTool() {
      
  }
  
  StatusCode MuonTrackToSegmentTool::initialize() {
    
    
    ATH_CHECK( m_propagator.retrieve() );
    ATH_CHECK( m_idHelperTool.retrieve() );
    ATH_CHECK( m_helperTool.retrieve() );
    ATH_CHECK( m_intersectSvc.retrieve() );
    ATH_CHECK( m_printer.retrieve() );
    return StatusCode::SUCCESS;
  }
  
  StatusCode MuonTrackToSegmentTool::finalize(){
    return StatusCode::SUCCESS;
  }

  MuonSegment* MuonTrackToSegmentTool::convert( const Trk::Track& track ) const {
    /** convert track to segment, express the new segment parameters on the surface of the first segment */

    ATH_MSG_DEBUG(" creating MuonSegment from track ");    

    const Trk::Perigee* perigee = track.perigeeParameters();
    if( !perigee ) {
      ATH_MSG_WARNING(" was expecting a perigee here... ");
      return 0;
    }
    
    const Trk::FitQuality* fq = track.fitQuality();
    if( !fq ){
      ATH_MSG_WARNING(" was expecting a FitQuality here... ");
      return 0;
    }

    std::set<Identifier> chIds;


    // copy rots, get surface 
    DataVector<const Trk::MeasurementBase>* rots = new DataVector<const Trk::MeasurementBase>();
    rots->reserve(track.measurementsOnTrack()->size());

    // loop over TSOS
    const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    if( !states ){
      ATH_MSG_WARNING(" track without states, discarding track ");
      return 0;
    }
    // track direction vector
    Amg::Vector3D dir = perigee->momentum().unit();

    const Amg::Transform3D* surfaceTransform = 0;
    const Amg::Transform3D* backupTransform = 0;
    const Amg::Transform3D* surfaceTransformToBeDeleted = 0;
    double weightedDistanceSquared = 0;
    double weightSquared = 0;
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
    for( ; tsit!=tsit_end ; ++tsit ){
      
      if( !*tsit ) continue; //sanity check
      
      // require TrackParameters
      const Trk::TrackParameters* pars = (*tsit)->trackParameters();
      if( !pars ) continue;
    
      // check whether state is a measurement
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if( !meas || (*tsit)->type(Trk::TrackStateOnSurface::Outlier) ) continue;
      rots->push_back( meas->clone());

      // only consider eta hits
      Identifier id = m_helperTool->getIdentifier(*meas);
      if( !id.is_valid() || m_idHelperTool->measuresPhi(id) ) continue;

      double distance = (pars->position()-perigee->position()).dot(dir);
      double weight = 1./meas->localCovariance()(Trk::locX,Trk::locX);
      ATH_MSG_VERBOSE(" distance " << distance << " error " << Amg::error(meas->localCovariance(),Trk::locX)
		      << " weight " << weight << " " << m_idHelperTool->toString(id));
      weightedDistanceSquared += distance*weight;
      weightSquared += weight;
      if( m_idHelperTool->isMdt(id) ){
	chIds.insert( m_idHelperTool->chamberId(id) );

	if( !surfaceTransform ){
	  const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(meas);
	  if( mdt ) {
	    // create new surface using AMDB reference frame
	    Amg::Transform3D* tmpSurfaceTransform = new Amg::Transform3D(mdt->detectorElement()->AmdbLRSToGlobalTransform().rotation());
	    tmpSurfaceTransform->pretranslate(mdt->detectorElement()->center());
	    surfaceTransform = tmpSurfaceTransform;
	    surfaceTransformToBeDeleted = surfaceTransform;
	  }
	}
      }else if( (m_idHelperTool->isMM(id) || m_idHelperTool->isCsc(id)) && !surfaceTransform ){
	surfaceTransform = &(meas)->associatedSurface().transform();
      }else if( !surfaceTransform && !backupTransform ) {
	backupTransform = &(meas)->associatedSurface().transform();
      }
    }
    if( !surfaceTransform ) surfaceTransform = backupTransform;
    // calculate distance new reference point, shift it 100 mm towards the start of the segment 
    double refDistance = weightedDistanceSquared/weightSquared-100;
    ATH_MSG_DEBUG(" weighted distance " << refDistance);
    
    Amg::Vector3D refPos = perigee->position() + refDistance*dir;
    
    // find closest measured parameters
    double minDist = 1e6;
    const Trk::TrackParameters* closestPars = 0;
    tsit = states->begin();
    tsit_end = states->end();
    for( ; tsit!=tsit_end ; ++tsit ){
      
      if( !*tsit ) continue; //sanity check
      
      // require TrackParameters
      const Trk::TrackParameters* pars = (*tsit)->trackParameters();
      if( !pars || !pars->covariance() ) continue;

      // look for the closest measured parameters to the reference point
      double distance = (pars->position()-refPos).dot(dir);
      if( distance < 0 && distance > minDist ){
	minDist = distance;
	closestPars = pars;
      }
    }

    if( !surfaceTransform ){
      ATH_MSG_WARNING(" failed to create a PlaneSurface for the track, cannot make segment!!! " << std::endl
	     << m_printer->print(track) << std::endl << m_printer->printStations(track));
      delete rots;
      return 0;
    }

    if( !closestPars ) {
      closestPars = perigee;
      minDist = (perigee->position()-refPos).dot(dir);
    }

    Amg::Transform3D* transform = new Amg::Transform3D(surfaceTransform->rotation());
    transform->pretranslate(refPos);
    double surfDim = 500.;
    Trk::PlaneSurface* surf = new Trk::PlaneSurface( transform, surfDim, surfDim );
    delete surfaceTransformToBeDeleted;

    const Trk::TrackParameters* exPars = m_propagator->propagate(*closestPars,*surf, minDist > 0 ? Trk::oppositeMomentum : Trk::alongMomentum ,
								 false,Trk::MagneticFieldProperties(Trk::NoField));
    if( !exPars || !exPars->covariance() ){
      ATH_MSG_DEBUG(" propagation failed!!! ");
      if( !surf->associatedDetectorElement() ) delete surf;
      delete exPars; // it is save to delete a NULL pointer
      delete rots;
      return 0;
    }

    Amg::Vector2D locPos;
    if( !surf->globalToLocal(exPars->position(),exPars->momentum(),locPos) ){
      ATH_MSG_WARNING(" localToGlobal failed!!! ");
      if( !surf->associatedDetectorElement() ) delete surf;
      delete exPars;
      delete rots;
      return 0;
    }
    Trk::LocalDirection locDir;
    surf->globalToLocalDirection(exPars->momentum(),locDir);

    // convert errors on global angles theta/phi to errors on local angles angleYZ/angleXZ
    Trk::JacobianPhiThetaLocalAngles globalToLocalMeasAnglesJacobian( exPars->parameters()[Trk::phi],
                                                                      exPars->parameters()[Trk::theta],
                                                                      exPars->associatedSurface().transform().rotation().inverse() );

    // make the Jacobian to convert all in one go from global to local
    // so that the correlations are calculated correctly
    AmgSymMatrix(5) globalToLocalMeasJacobian;
    globalToLocalMeasJacobian.setZero();
    globalToLocalMeasJacobian(Trk::locX,Trk::locX) = 1.0;
    globalToLocalMeasJacobian(Trk::locY,Trk::locY) = 1.0;
    globalToLocalMeasJacobian(Trk::phi,Trk::phi) = globalToLocalMeasAnglesJacobian(0,0);
    globalToLocalMeasJacobian(Trk::theta,Trk::theta) = globalToLocalMeasAnglesJacobian(1,1);
    globalToLocalMeasJacobian(Trk::theta,Trk::phi) = globalToLocalMeasAnglesJacobian(0,1); // also fills (Trk::phi,Trk::theta)
    globalToLocalMeasJacobian(Trk::phi,Trk::theta) = globalToLocalMeasJacobian(Trk::theta,Trk::phi); // also fills (Trk::theta,Trk::phi)
    globalToLocalMeasJacobian(Trk::qOverP,Trk::qOverP) = 1.0;
    
    AmgSymMatrix(5) cov = exPars->covariance()->similarity(globalToLocalMeasJacobian);
    
    Trk::FitQuality* quality = 0;
    if( !chIds.empty() ){
      // calculate holes
      std::vector<Identifier> holes;
      std::set<Identifier>::iterator cit = chIds.begin();
      std::set<Identifier>::iterator cit_end = chIds.end();
      for( ;cit!=cit_end;++cit ){
	std::vector<Identifier> holesChamber = calculateHoles( *cit, *exPars, rots->stdcont() );
	holes.insert(holes.end(),holesChamber.begin(),holesChamber.end()); 
      } 
      
      quality = new MuonSegmentQuality( fq->chiSquared(), fq->numberDoF(), holes );

    }else{
      quality = new Trk::FitQuality( fq->chiSquared(), fq->numberDoF() );
    }
    MuonSegment* seg = new MuonSegment(locPos,locDir,cov,surf,rots,quality);
    delete exPars;

    return seg;
  }

  std::vector<Identifier> MuonTrackToSegmentTool::calculateHoles( const Identifier& chid, const Trk::TrackParameters& pars, 
								  const MuonTrackToSegmentTool::MeasVec& measurements ) const {
    
    // calculate crossed tubes
    const MuonStationIntersect& intersect = m_intersectSvc->tubesCrossedByTrack( chid, pars.position(), pars.momentum().unit() );


    // set to identify the hit on the segment
    std::set<Identifier> hitsOnSegment;
    MeasCit mit = measurements.begin();
    MeasCit mit_end = measurements.end();
    for( ;mit!=mit_end;++mit ){
      const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(*mit);
      if( mdt ) hitsOnSegment.insert( mdt->identify() );
    }

    // clear hole vector
    std::vector<Identifier> holes;
    for( unsigned int ii=0;ii<intersect.tubeIntersects().size();++ii ){
      const MuonTubeIntersect& tint = intersect.tubeIntersects()[ii];

      if( hitsOnSegment.count( tint.tubeId ) ) {
	continue;
      }
      if( fabs( tint.rIntersect ) > 14.4 || tint.xIntersect > -200. ){
      }else{
	// check whether there is a hit in this tube 

	holes.push_back( tint.tubeId );
      }
    }
    return holes;
  }


}
