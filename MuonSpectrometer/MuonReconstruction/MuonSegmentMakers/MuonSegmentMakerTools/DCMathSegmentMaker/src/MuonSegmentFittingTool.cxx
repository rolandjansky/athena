/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentFittingTool.h"

#include "GaudiKernel/MsgStream.h"

#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkEventPrimitives/DefinedParameter.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/FitQuality.h"

#include "TrkTrack/Track.h"

#include "TrkParameters/TrackParameters.h"

#include "TrkSurfaces/PlaneSurface.h"

#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkExInterfaces/IPropagator.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecToolInterfaces/IMuonTrackCleaner.h"

#include "MuonSegment/MuonSegment.h"

namespace Muon {

  MuonSegmentFittingTool::MuonSegmentFittingTool( const std::string& t, const std::string& n, const IInterface*  p ) :
    AthAlgTool(t,n,p),
    m_slPropagator("Trk::RungeKuttaPropagator/AtlasRungeKuttaPropagator"),
    m_magFieldProperties(Trk::NoField),
    m_slTrackFitter("Trk::GlobalChi2Fitter/MCTBSLFitter"),
    m_curvedTrackFitter("Trk::GlobalChi2Fitter/MCTBFitter"),
    m_trackCleaner("Muon::MuonTrackCleaner/MuonTrackCleaner"),
    m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool")
  {
    declareInterface<IMuonSegmentFittingTool>(this);

    declareProperty("SLPropagator",   m_slPropagator);
    declareProperty("SLFitter",       m_slTrackFitter);
    declareProperty("CurvedFitter",   m_curvedTrackFitter);
    declareProperty("TrackCleaner",   m_trackCleaner);
    declareProperty("IdHelper",       m_idHelperTool);
    declareProperty("UpdatePrecisionCoordinate", m_updatePrecisionCoordinate = false );
  }

  MuonSegmentFittingTool::~MuonSegmentFittingTool()
  {
  }

  StatusCode MuonSegmentFittingTool::initialize()
  {
    
    if( AthAlgTool::initialize().isFailure() ) return StatusCode::FAILURE;
    if( m_slPropagator.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not get " << m_slPropagator); 
      return StatusCode::FAILURE;
    }

    if( m_idHelperTool.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not get " << m_idHelperTool); 
      return StatusCode::FAILURE;
    }    

    if( m_slTrackFitter.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not get " << m_slTrackFitter); 
      return StatusCode::FAILURE;
    }    


    if( m_curvedTrackFitter.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not get " << m_curvedTrackFitter); 
      return StatusCode::FAILURE;
    }    

    if( m_trackCleaner.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not get " << m_trackCleaner); 
      return StatusCode::FAILURE;
    }    

    return StatusCode::SUCCESS;
  }

  StatusCode MuonSegmentFittingTool::finalize()
  {
    return AthAlgTool::finalize();
  }

  Trk::Track* MuonSegmentFittingTool::fit( const MuonSegment& segment ) const {
    return fit( segment.globalPosition(),segment.globalDirection(),segment.associatedSurface(),segment.containedMeasurements());
  }

  Trk::Track* MuonSegmentFittingTool::fit( const Amg::Vector3D& gpos, const Amg::Vector3D& gDir, const Trk::PlaneSurface& surf, 
					   const std::vector<const Trk::MeasurementBase*>& rioVec ) const {
    
    
    ATH_MSG_VERBOSE(" trying to fit segment ");
    Amg::Vector3D gdir = gDir;

    double gdirNorm = gdir.mag();
    bool isCurvedSegment = ( gdirNorm > 1.5 ? true : false ); 
    if(isCurvedSegment) {
      //re-normalize the gdir
      gdir = Amg::Vector3D(gdir.x()/gdirNorm,gdir.y()/gdirNorm,gdir.z()/gdirNorm);
    }

    double charge = 0;
    Trk::AtaPlane segPars(gpos,gdir,charge,surf);
 
    // extrapolate segment parameters to first measurements
    const Trk::MeasurementBase* firstMeas = rioVec.front(); 
    const Trk::TrackParameters* exPars = m_slPropagator->propagate(segPars,firstMeas->associatedSurface(),Trk::anyDirection,false,m_magFieldProperties);
    if( !exPars ) {
      ATH_MSG_DEBUG(" Propagation failed!! ");
      return 0;
    }

    // small shift towards the ip
    double sign = exPars->position().dot(exPars->momentum()) > 0 ? 1. : -1.;
    Amg::Vector3D perpos =  exPars->position() - sign*exPars->momentum().unit();
    delete exPars; exPars = 0;

    // create start parameter
    double phi=gdir.phi();
    double theta=gdir.theta();
    double qoverp=0;
    Trk::PerigeeSurface persurf(perpos);
    Trk::Perigee startpar(0,0,phi,theta,qoverp,persurf);

    // copy measurements into new vector
    std::vector<const Trk::MeasurementBase*> vec2;
    vec2.reserve(rioVec.size());
    std::copy( rioVec.begin(), rioVec.end(), std::back_inserter(vec2) );
  
    // fit     
    Trk::ParticleSwitcher particleSwitch;
    Trk::Track* newtrack;
    //use the full fitter if the segment is curved
    if(isCurvedSegment) {                
      newtrack = m_curvedTrackFitter->fit(vec2,startpar,false,particleSwitch.particle[0]);
    }
    //else use the straight line fitter
    else {
      newtrack = m_slTrackFitter->fit(vec2,startpar,false,Trk::nonInteracting);
    }

    if( !newtrack ) {
      ATH_MSG_VERBOSE("     fit failed ");
      return 0;
    }

    Trk::Track* cleanTrack = m_trackCleaner->clean(*newtrack);
    if( !cleanTrack && !isCurvedSegment ) {
      ATH_MSG_VERBOSE("     lost in cleaner ");
      delete newtrack;
      return 0;
    }

    if( cleanTrack != newtrack && !isCurvedSegment ) {
      delete newtrack;
      newtrack = cleanTrack;
    }
    
    const Trk::FitQuality* fq = newtrack->fitQuality();
    if( !fq ) {
      // no fit quality!!, discard track
      ATH_MSG_WARNING(" track without fit quality!! ");
      delete newtrack;
      return 0;
    }
    
    double reducedChi2 = fq->chiSquared()/fq->numberDoF();
    double cut = 10.;
    // reject fit if larger than cut
    if( reducedChi2 > cut ) {
      ATH_MSG_VERBOSE("     reduced chi2 to large " << reducedChi2 << "  cut  " << cut);
      delete newtrack;
      return 0;
    }
	
    if( msgLvl(MSG::DEBUG) ){
      msg(MSG::DEBUG) << MSG::DEBUG << std::setprecision(5) << " chi2 " << fq->chiSquared() << " ndof " << fq->numberDoF();
      const Trk::Perigee* pp = newtrack->perigeeParameters();
      if( pp ){
	msg(MSG::DEBUG) << " pos " << std::setprecision(5) << pp->position() 
			<< " phi " << pp->momentum().phi() << " theta " << pp->momentum().theta() 
			<< " q*mom " << pp->momentum().mag()*pp->charge() 
			<< " pt " << pp->momentum().perp()<< endmsg;
      }else{
	msg(MSG::DEBUG) << " no perigee " << endmsg;
      }
    }
    return newtrack;
  }


  void MuonSegmentFittingTool::updateSegmentParameters( const Trk::Track& track, const Trk::PlaneSurface& surf, Amg::Vector2D& segLocPos, 
							Trk::LocalDirection& segLocDir, Amg::MatrixX& locerr ) const
  {
 
    ATH_MSG_DEBUG(" old segment parameters: pos (" << segLocPos[Trk::locX] << "," << segLocPos[Trk::locY] << ")  dir ("
		  << segLocDir.angleXZ() << "," << segLocDir.angleYZ() << ") ");
  
    
    const Trk::Perigee* pp = track.perigeeParameters();
    if( !pp ){
      ATH_MSG_WARNING(" track without perigee ");
      return;
    }

    const Trk::TrackParameters* exPars = m_slPropagator->propagate( *pp,surf,Trk::anyDirection,false,m_magFieldProperties);
    if( !exPars ){
      ATH_MSG_WARNING(" extrapolation failed, this should not happen ");
      return;
    }

    Amg::Vector2D lpos;
    surf.globalToLocal( exPars->position(),exPars->position(),lpos );
    Trk::LocalDirection ldir;
    surf.globalToLocalDirection( exPars->momentum().unit(), ldir );
    ATH_MSG_DEBUG(" new segment parameters: pos (" << lpos[Trk::locX] << "," << lpos[Trk::locY] << ")  dir ("
			 << ldir.angleXZ() << "," << ldir.angleYZ() << ") ");

    segLocPos[Trk::locX] = lpos[Trk::locX];
    if( m_updatePrecisionCoordinate ) segLocPos[Trk::locY] = lpos[Trk::locY];
    double& locAngleXZ = const_cast<double&>(segLocDir.angleXZ());
    double& locAngleYZ = const_cast<double&>(segLocDir.angleYZ());
    locAngleXZ = ldir.angleXZ();
    if( m_updatePrecisionCoordinate ) locAngleYZ = ldir.angleYZ();
    
    if(exPars->covariance())  locerr = *exPars->covariance();

    delete exPars;
  }

}

