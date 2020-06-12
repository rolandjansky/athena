/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuTagMatchingTool.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkSurfaces/Surface.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "TrkExInterfaces/IExtrapolator.h"  
#include "TrkExInterfaces/IPropagator.h"  
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkEventPrimitives/JacobianPhiThetaLocalAngles.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentQuality.h"
#include "MuonCombinedEvent/MuonSegmentInfo.h"

#include <cmath>

namespace {
  // local helper functions
#if 0
  /** Limit deltaPhi between -pi < dPhi <= +pi */
  inline double limit_deltaPhi(double deltaPhi) {
    while (deltaPhi >  +M_PI) deltaPhi -= 2.*M_PI;
    while (deltaPhi <= -M_PI) deltaPhi += 2.*M_PI;
    return deltaPhi;
  }

  /** Limit deltaTheta between -pi/2 < dPhi <= +pi/2 */
  inline double limit_deltaTheta(double deltaTheta) {
    while (deltaTheta >  +M_PI/2.) deltaTheta -= M_PI;
    while (deltaTheta <= -M_PI/2.) deltaTheta += M_PI;
    return deltaTheta;
  }
#endif

}


MuTagMatchingTool::MuTagMatchingTool(const std::string& t,
				     const std::string& n,
				     const IInterface*  p ):
  AthAlgTool(t,n,p)
  , p_IExtrapolator("Trk::Extrapolator/AtlasExtrapolator")
  , p_propagator("Trk::RungeKuttaPropagator/AtlasRungeKuttaPropagator")
  , m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool")
  , m_hitSummaryTool("Muon::MuonSegmentHitSummaryTool/MuonSegmentHitSummaryTool")
  , m_selectionTool("Muon::MuonSegmentSelectionTool/MuonSegmentSelectionTool")
  , m_pullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator")
{
  declareInterface<IMuTagMatchingTool>(this);
  declareProperty( "IExtrapolator" , p_IExtrapolator ) ;
  declareProperty( "Propagator" , p_propagator ) ;

  declareProperty( "GlobalThetaCut",    m_GLOBAL_THETA_CUT       = 0.1  );
  declareProperty( "GlobalPhiCut",      m_GLOBAL_PHI_CUT         = 0.5  );
  declareProperty( "GlobalRCut",        m_GLOBAL_R_CUT           = 1000. ); 

  declareProperty( "ThetaCut",          m_MATCH_THETA            = 5. ); // pull cut in local precision position (bending plane) 
  declareProperty( "PhiCut",            m_MATCH_PHI              = 30. ); // pull cut in local NON precision position  
  declareProperty( "ThetaAngleCut",     m_MATCH_THETAANGLE       = 5. ); // pull cut in local precision direction  
  declareProperty( "PhiAngleCut",       m_MATCH_PHIANGLE         = 30. ); // pull cut in local NON precision direction

  declareProperty( "ErrorProtectionPosCut", m_ERROR_EX_POS_CUT   = 5000. );
  declareProperty( "ErrorProtectionDirCut", m_ERROR_EX_ANGLE_CUT = 0.35  );

  declareProperty( "DoDistanceCut",     m_doDistCut            = true  );
  declareProperty( "PositionDistance",  m_DIST_POS               = 2000. );  
  declareProperty( "AngleDistance",     m_DIST_ANGLE             =   0.7 );  

  declareProperty( "ThetaSafety",       m_SAFE_THETA             = 25.    );  // additional uncertainty on precision plane position (bendig plane) 
  declareProperty( "PhiSafety",         m_SAFE_PHI               = 100.    ); //  additional uncertainty on NON precision plane position
  declareProperty( "ThetaAngleSafety",  m_SAFE_THETAANGLE        = 0.0015 ); //  additional uncertainty on precision plane direction
  declareProperty( "PhiAngleSafety",    m_SAFE_PHIANGLE          = 0.25   ); //  additional uncertainty on NON precision plane direction

  declareProperty( "ChamberPullCut", m_chamberPullCut = 5. ); // Inside chamber pull is negative for outside chamber pull > 5 standard deviations (from edge) in non precision plane
  declareProperty( "CombinedPullCut", m_combinedPullCut = 5. ); // Combined pull build from local precision position and angle including the correlation 
 
 
  declareProperty( "AssumeLocalErrors", m_assumeLocalErrors    = true );
  declareProperty( "ExtrapolatePerigee",m_extrapolatePerigee   = false );
  declareProperty( "MuonChamberT0s", m_t0Location = "MooreMuonChamberT0s" );
}

StatusCode MuTagMatchingTool::initialize()
{  
  ATH_MSG_DEBUG( "================================" );
  ATH_MSG_DEBUG( "=Proprieties are " );
  ATH_MSG_DEBUG( "GlobalThetaCut"   << std::setw(10) << m_GLOBAL_THETA_CUT);   
  ATH_MSG_DEBUG( "GlobaPhiCut"      << std::setw(10) << m_GLOBAL_PHI_CUT  );   
  ATH_MSG_DEBUG( "GlobalRCut"       << std::setw(10) << m_GLOBAL_R_CUT    );   
  ATH_MSG_DEBUG( "ThetaCut"         << std::setw(10) << m_MATCH_THETA     );   
  ATH_MSG_DEBUG( "PhiCut"           << std::setw(10) << m_MATCH_PHI       );   
  ATH_MSG_DEBUG( "ThetaAngleCut"    << std::setw(10) << m_MATCH_THETAANGLE);   
  ATH_MSG_DEBUG( "PhiAngleCut"      << std::setw(10) << m_MATCH_PHIANGLE  );      
  ATH_MSG_DEBUG( "DoDistanceCut"    << std::setw(10) << m_doDistCut     );      
  ATH_MSG_DEBUG( "ThetaDistance"    << std::setw(10) << m_DIST_POS      );      
  ATH_MSG_DEBUG( "ThetaAngleDistance"<< std::setw(10)<< m_DIST_ANGLE );      
  ATH_MSG_DEBUG( "ThetaSafety"      << std::setw(10) << m_SAFE_THETA      );      
  ATH_MSG_DEBUG( "PhiSafety"        << std::setw(10) << m_SAFE_PHI        );        
  ATH_MSG_DEBUG( "ThetaAngleSafety" << std::setw(10) << m_SAFE_THETAANGLE );   
  ATH_MSG_DEBUG( "PhiAngleSafety"   << std::setw(10) << m_SAFE_PHIANGLE   );      
  
  ATH_MSG_DEBUG( "AssumeLocalErrors "<< std::setw(10) << m_assumeLocalErrors );  
  ATH_MSG_DEBUG( "ExtrapolatePerigee "<< std::setw(10) << m_extrapolatePerigee );  
  ATH_MSG_DEBUG( "================================" );

//Retrieve p_IExtrapolator
  ATH_CHECK( p_IExtrapolator.retrieve() );
  ATH_MSG_DEBUG( "Retrieved tool " << p_IExtrapolator );

  ATH_CHECK( p_propagator.retrieve() );
  
// MuonDetectorManager from the conditions store
  ATH_CHECK(m_DetectorManagerKey.initialize());

  ATH_CHECK( m_idHelperSvc.retrieve() );
  ATH_CHECK( m_edmHelperSvc.retrieve() );
  ATH_CHECK( m_printer.retrieve() );
  ATH_CHECK( m_pullCalculator.retrieve() );
  
  return StatusCode::SUCCESS;
}

bool MuTagMatchingTool::match( const Trk::TrackParameters*       atSurface, 
				    const Muon::MuonSegment*     segment, 
				    std::string                  surfaceName ) const {
  ///////////////////////////////////////////////////////////////////////////
  // performs a rough match to an abstract MS surface (disc or cylinder)
  // by matching on station name (I M O, S L, B E) and a crude phi-position
  // match, followed by either a match in theta-position or r-position
  ///////////////////////////////////////////////////////////////////////////
  if( !atSurface ) return false;
  if( !segment ) return false;
  
  if( !surfaceMatch( atSurface,
		     segment, 
		     surfaceName ) ) return false;

  if( !phiMatch( atSurface,
		 segment, 
		 surfaceName  ) ) return false;

  if( surfaceName.find('B') != std::string::npos )  return thetaMatch( atSurface, segment );
  else return rMatch( atSurface, segment );

}

std::string MuTagMatchingTool::segmentStationString( const Muon::MuonSegment* segment ) const {
  std::string station;

  for( unsigned int i = 0; i<segment->numberOfContainedROTs(); ++i ){
    const Trk::RIO_OnTrack* rot=segment->rioOnTrack(i);
    if(!rot){
      ATH_MSG_DEBUG("no ROT");
      continue;
    }
    Identifier segID = segment->rioOnTrack(i)->identify();
    if( m_idHelperSvc->isMdt(segID) ){
      station = m_idHelperSvc->mdtIdHelper().stationNameString( m_idHelperSvc->mdtIdHelper().stationName( segID ) );
      break;
    } else if( m_idHelperSvc->isCsc(segID) ){
      station = m_idHelperSvc->cscIdHelper().stationNameString( m_idHelperSvc->cscIdHelper().stationName( segID ) );
      break ;
    }
  }
  return station;
}

bool  MuTagMatchingTool::surfaceMatch( const Trk::TrackParameters*  atSurface, 
				       const Muon::MuonSegment*     segment, 
				       std::string                  surfaceName ) const {
  std::string segStation( segmentStationString( segment ) );

  ///  ATH_MSG_DEBUG( "Matching segmentstation " << segStation << " to surface " << surfaceName );
  
  if( surfaceName[0] == 'B' && segStation[0] == 'B' ){
    if( surfaceName[1] == segStation[1] ) return true ;
    else if( (surfaceName[1] == 'I') && (segStation[1] == 'E') ) return true ;
    else return false;
  }
  else if( surfaceName[0] == 'E' && segStation[0] == 'E' ){
    if(  surfaceName[1] != segStation[1] ) return false ;
    double exTrkZ( atSurface->position().z() );
    if( ( (surfaceName[2] == 'A') && (exTrkZ < 0) ) || ( ( surfaceName[2] == 'C') && (exTrkZ > 0) ) ) return false ;       
    else return true;
  }
  else if( surfaceName[0] == 'E' && segStation[0] == 'T' ){
    double segZ( segment->globalPosition().z() );  
    if( surfaceName.find('A') != std::string::npos ){
      if( surfaceName.at(1) == 'I'  && ( segZ > 6000. ) && ( segZ < 8000. ) ) return true;
      else if( surfaceName.at(1) == 'M' && ( segZ > 12000. ) && ( segZ < 16000. ) ) return true;
    } else if ( surfaceName.find('C') != std::string::npos ){
      if( surfaceName.at(1) == 'I'  && ( segZ < -6000.)  && ( segZ > -8000. )  ) return true;
      else if( surfaceName.at(1) == 'M' && ( segZ < -12000.)  && ( segZ > -16000.) ) return true;
    } else return false;
  }
  else if( surfaceName[0] == 'E' && segStation[0] == 'C'){
    double segZ( segment->globalPosition().z() );  
    double exTrkZ( atSurface->position().z() );
    if( segZ*exTrkZ < 0. ) return false ;
    else return true;
  } 
  //  else ATH_MSG_DEBUG( "NOT Passing surface match, couldn't find reason not to." << surfaceName << " and " << segStation );
  return false;

}

/** Get extrapolation at MS entrance: this will be the starting point to following extrapolations*/
const Trk::TrackParameters* MuTagMatchingTool::ExtrapolateTrktoMSEntrance(
									 const Trk::Track* pTrack,
									 Trk::PropDirection direction
									 ) const {
  if ( pTrack == 0 ) return 0 ; 

  StatusCode sc;
 
  const Trk::TrackingGeometry *trackingGeometry=0;
 
 
  sc = detStore()->retrieve(trackingGeometry,"AtlasTrackingGeometry");
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Could not find tracking geometry. Exiting." );
    return 0;
  }

  const Trk::TrackParameters* exTrk = 0 ;
  const Trk::TrackingVolume* MSEntranceVolume = trackingGeometry->trackingVolume("MuonSpectrometerEntrance");

//  if( m_extrapolatePerigee ){
  exTrk = p_IExtrapolator->extrapolateToVolume( *( pTrack->perigeeParameters() ),
						  *MSEntranceVolume, 
						  direction, 
						  Trk::muon ) ;
//  } else { // using perigee parameters arbitrarily; to be changed if new extrapolateToVolume is available
//    exTrk = p_IExtrapolator->extrapolateToVolume( *( pTrack->trackParameters() ), 
//						  *MSEntranceVolume, 
//						  direction, 
//						  Trk::muon ) ;
//  }
  if( !exTrk ) ATH_MSG_DEBUG( "Track could not be extrapolated to MS entrance..." );
  else ATH_MSG_DEBUG( "Track extrapolated to MS entrance Radius "<< exTrk->position().perp() << " Z " <<  exTrk->position().z());

  return exTrk;
}

/** Get extrapolation at MSSurface level*/
const Trk::TrackParameters* MuTagMatchingTool::ExtrapolateTrktoMSSurface(
									 const Trk::Surface* pSurface,
									 const Trk::TrackParameters* pTrack,
									 Trk::PropDirection direction
									 ) const {
  if ( pSurface == 0 ) return 0 ;
  if ( pTrack == 0 ) return 0 ; 
  
  const Trk::TrackParameters* exTrk = p_IExtrapolator->extrapolate( *pTrack,
								    *pSurface, 
								    direction, 
								    false, 
								    Trk::muon ) ;
  if( !exTrk ) {
    ATH_MSG_DEBUG( " didn't manage to extrapolate TrackParameters to abstract surface Radius " << pSurface->center().perp() << " Z " << pSurface->center().z() );
    return 0;
  }
  return exTrk;
}

const Trk::Perigee* MuTagMatchingTool::flipDirection( const Trk::Perigee* inputPars ) const {
  //  return inputPars->clone();
  // CLHEP::HepVector pars = inputPars->parameters();
  const AmgVector(5)& pars = inputPars->parameters();
  double pi(std::acos(-1));
  double flippedPhi = pars[2] + pi;
  if( flippedPhi > pi) flippedPhi -= 2*pi;
  double flippedTheta = pi - pars[3] ;
  if( flippedTheta < 0. ) flippedTheta += pi;
 // Trk::ErrorMatrix* errorMat = new Trk::ErrorMatrix( inputPars->localErrorMatrix() );
  if(inputPars->covariance()){
  AmgSymMatrix(5)* covMat = new AmgSymMatrix(5)(*inputPars->covariance());
  const Trk::PerigeeSurface perSurf;
  return new Trk::Perigee( -pars[0], pars[1], flippedPhi, flippedTheta, pars[4],perSurf, covMat ) ;
   }
  else{
    ATH_MSG_DEBUG ("flipDirection: no covariance associated to input parameters " << *inputPars);
    return NULL;
  }
}

/** Get extrapolation at Segment Plane Surface level*/
const Trk::AtaPlane* MuTagMatchingTool::ExtrapolateTrktoSegmentSurface(
									       const Muon::MuonSegment*  segment,
									       const Trk::TrackParameters* pTrack,
									       Trk::PropDirection direction
									       ) const {
  if ( segment == 0 ) return 0 ;
  if ( pTrack == 0 ) return 0 ; 
 
  const  Trk::AtaPlane*  matap = 0;
  
  bool isCsc( isCscSegment(segment) );
  unsigned int hits( cscHits(segment) );
  if( isCsc){
    ATH_MSG_DEBUG( "CSC segment has " << hits << " hits" );
    if( hits<5 ){
      ATH_MSG_DEBUG( "CSC segment is identified to be 2D. Not extrapolating." );
      return matap;
    }
  }

  const Trk::TrackParameters* atap =  p_IExtrapolator->extrapolate( *pTrack,
								    segment->associatedSurface(), 
								    direction, 
								    false, 
								    Trk::muon ) ; 
 
  if( atap ){
    matap = dynamic_cast< const Trk::AtaPlane* >( atap );
  } else ATH_MSG_DEBUG( " didn't manage to extrapolate TrackParameters to segment surface Radius" << segment->associatedSurface().center().perp() << " Z " << segment->associatedSurface().center().z() );
  
  if( !matap ){
    if(atap) ATH_MSG_DEBUG( " didn't manage to extrapolate MeasuredTrackParameters to segment surface atap Radius " << atap->position().perp() << " Z" << atap->position().z() );
    delete atap;
  }
  if(matap) ATH_MSG_DEBUG( " Succesfull extrapolation segment surface matap Radius " << matap->position().perp() << " Z" << matap->position().z() ); 
  return matap;
}

bool MuTagMatchingTool::phiMatch( const Trk::TrackParameters*  atSurface, 
				  const Muon::MuonSegment*     segment, 
				  std::string                  surfaceName ) const {
  
  double PHI_CUT = m_GLOBAL_PHI_CUT; 
  if( hasPhi(segment) ) PHI_CUT = m_GLOBAL_PHI_CUT/2. ; 

  Amg::Vector3D exTrkPos = atSurface->position();
  Amg::Vector3D segPos = segment->globalPosition();
  
  double dotprod = exTrkPos.x() * segPos.x()+ exTrkPos.y() * segPos.y() ;
  double cosphi = dotprod / (exTrkPos.perp() * segPos.perp() ) ;

  double sigma_phi = 0.;
  if( surfaceName.find('B') != std::string::npos ){ //error of barrelstation
    const Trk::AtaCylinder* atCylSurf = dynamic_cast< const Trk::AtaCylinder* >(atSurface);
    if( atCylSurf && atCylSurf->covariance() ){
      //  const Trk::ErrorMatrix localErrors = atCylSurf->localErrorMatrix();
      const AmgSymMatrix(5)& covAtCyl = *atCylSurf->covariance();
      double r = atSurface->associatedSurface().bounds().r();
      //     if( ( r != 0. ) && ( localErrors.covValue(Trk::locRPhi) >= 0 ) ) //retrieve the error only if covariance is ok
	       //sigma_phi = std::sqrt( localErrors.error(Trk::locRPhi) / r );
      if( (r!=0.) && (covAtCyl(Trk::locRPhi,Trk::locRPhi)>=0.))
	sigma_phi = Amg::error(covAtCyl,Trk::locRPhi) / r ;
    } else ATH_MSG_DEBUG( " track not extrapolated to a cylinder " );

  } else {//error in endcap
    const Trk::AtaDisc* atDiscSurf = dynamic_cast< const Trk::AtaDisc* >(atSurface);
    if( atDiscSurf && atDiscSurf->covariance() ){
      // const Trk::ErrorMatrix localErrors = atDiscSurf->localErrorMatrix();
      const AmgSymMatrix(5)& covAtDisc = *atDiscSurf->covariance();
      // if( localErrors.covValue(Trk::locPhi) >= 0 ) //retrieve the error only if covariance is ok
	   // sigma_phi = localErrors.error(Trk::locPhi) ;
      if( covAtDisc(Trk::locPhi,Trk::locPhi) >= 0. )
	sigma_phi = Amg::error(covAtDisc,Trk::locPhi);
    } else ATH_MSG_DEBUG( " track not extrapolated to a disc " );
  } 
  
  double errPhi =  std::sqrt( PHI_CUT*PHI_CUT + sigma_phi*sigma_phi ) ;
  
  //if the difference between exTrk and Segment phi position falls within the errPhi, accept as rough match
//  if( std::acos(cosphi) < errPhi ) return true;
  if( std::acos(fabs(cosphi)) < errPhi ) return true; //BRes: TEMPORARY - segment direction not sure, so I'm making this match symmetric wrt Pi/2
//  else ATH_MSG_DEBUG( std::setw(30) << "roughPhi failed:  d_phi = " << std::setw(10) << std::acos(cosphi)
  else ATH_MSG_DEBUG( std::setw(30) << "roughPhi failed:  d_phi = " << std::setw(10) << std::acos(fabs(cosphi)) //BRes: TEMPORARY - same 
	   << " while the cut is set on " << std::setw(10) <<errPhi );
  return false;
}

bool MuTagMatchingTool::hasPhi( const Muon::MuonSegment* seg ) const {
//  double phi = seg->globalDirection().phi();
//  double x = seg->globalPosition().x();
//  double y = seg->globalPosition().y();
//  double phisp = atan2(y,x);
//  double dotprod = std::cos(phi)*std::cos(phisp)+std::sin(phi)*std::sin(phisp);
    
//  if (dotprod > 0.9999) {
//    return false; //direction's phi and position's phi match too good: this can't be a phi measurement
//  } else return true;

   Muon::IMuonSegmentHitSummaryTool::HitCounts hitCounts = m_hitSummaryTool->getHitCounts(*seg);
   if (hitCounts.nexpectedTrigHitLayers> 1) {
     return true;
   } else {
     return false;
   }
}

bool MuTagMatchingTool::thetaMatch( const Trk::TrackParameters*  atSurface, 
				    const Muon::MuonSegment*     segment ) const {
  double THETA_CUT = m_GLOBAL_THETA_CUT;
  Amg::Vector3D exTrkPos = atSurface->position();
  Amg::Vector3D segPos = segment->globalPosition();
  
  if( fabs( exTrkPos.theta() - segPos.theta() ) < THETA_CUT ) return true;
  else ATH_MSG_DEBUG( std::setw(30) << "roughTheta failed: d_theta = " << std::setw(10)
	   << fabs( exTrkPos.theta() - segPos.theta() ) << " while the cut is set on " << std::setw(10) << THETA_CUT );
  return false ;
}

bool MuTagMatchingTool::rMatch( const Trk::TrackParameters*  atSurface, 
				const Muon::MuonSegment*     segment ) const {
  
  Amg::Vector3D exTrkPos = atSurface->position();
  double L = exTrkPos.mag();
  double R_CUT = m_GLOBAL_R_CUT* (L/7500.) ; //mm

  Amg::Vector3D segPos = segment->globalPosition();
  
  if( fabs( exTrkPos.perp() - segPos.perp() ) < R_CUT ) return true;
  else ATH_MSG_DEBUG( std::setw(30) << "rough  R   failed:   d_R    = " << std::setw(10) 
	   <<  fabs( exTrkPos.perp() - segPos.perp() )<< " while the cut is set on " << std::setw(10) << R_CUT  );
  return false ;
}

double MuTagMatchingTool::errorProtection( double exTrk_Err, bool isAngle ) const {

  double newError = exTrk_Err ;
 
  if( !isAngle && (exTrk_Err > m_ERROR_EX_POS_CUT)   ) newError = m_ERROR_EX_POS_CUT;
  if(  isAngle && (exTrk_Err > m_ERROR_EX_ANGLE_CUT) ) newError = m_ERROR_EX_ANGLE_CUT ;

  return newError;

}

bool MuTagMatchingTool::matchSegmentPosition( MuonCombined::MuonSegmentInfo* info,  bool idHasEtaHits) const {

  if(!info) {
    ATH_MSG_DEBUG( " No MuTagSegmentInfo matchSegmentPosition" );
    return false;
  } 
  if( !info->segment || !info->trackAtSegment ){
      ATH_MSG_DEBUG( " No segment and or trackAtSegment pointer matchSegmentPosition ");
    return false;
  }

  bool pass(false); 

  double scale = 1.;
//  if(info->stationLayer==12) scale = 5./3.;

  if( idHasEtaHits ) {
    if (fabs(info->pullY) < m_MATCH_THETA || fabs(info->pullCY) < scale*m_combinedPullCut ) pass = true;
  } else {
    if (fabs(info->pullX) < m_MATCH_PHI) pass = true; 
  }

  return pass;
}
bool MuTagMatchingTool::matchSegmentDirection( MuonCombined::MuonSegmentInfo* info,  bool idHasEtaHits) const {

  if(!info) {
    ATH_MSG_DEBUG( " No MuTagSegmentInfo matchSegmentDirection " );
    return false;
  } 
  if( !info->segment || !info->trackAtSegment ){
      ATH_MSG_DEBUG( " No segment and or trackAtSegment pointer matchSegmentDirection ");
    return false;
  }

  double scale = 1.;
//  if(info->stationLayer==12) scale = 5./3.;

  if( idHasEtaHits ) {
    if (fabs(info->pullYZ) < m_MATCH_THETAANGLE || fabs(info->pullCY) < scale*m_combinedPullCut ) {
      return true; 
    } else {return false;}
  } else {
     return true; 
//    if (fabs(info->pullXZ) < m_MATCH_PHIANGLE) return true; 
  }

}
  

bool MuTagMatchingTool::matchCombinedPull( MuonCombined::MuonSegmentInfo* info ) const {
  bool pass(true);
  if(!info) {
    ATH_MSG_DEBUG( " No MuTagSegmentInfo matchCombinedPull " );
    return false;
  } 
  if( !info->segment || !info->trackAtSegment ){
    ATH_MSG_DEBUG( " No segment and or trackAtSegment pointer matchCombinedPull ");
    return false;
  }

  if(info->pullChamber > m_chamberPullCut ) pass = false;
  if(fabs(info->pullCY) > m_combinedPullCut ) pass = false;
//
// if segment could have phi hits and has phi hits ( fabs(info->minimumPhiPull) < 100000.) cut at 5 sigma
// if NO phits the chamber pull cut requires that the track passes the chamber within 5 sigma
//
//  if(info->hasPhi && (fabs(info->minimumPullPhi) > m_combinedPullCut && fabs(info->minimumPullPhi) < 100000. ) ) pass = false;
   ATH_MSG_DEBUG( " matchCombinedPull MuTagSegmentInfo hasPhi " << info->hasPhi << " minimumPullPhi " << info->minimumPullPhi << " pullChamber " << info->pullChamber << " cut " << m_chamberPullCut << " pullCY " << info->pullCY << " cut m_combinedPullCut " << m_combinedPullCut << " pass " << pass );

  
 
  return pass;
 
}

bool MuTagMatchingTool::matchPtDependentPull( MuonCombined::MuonSegmentInfo* info,  
					const Trk::Track*  trk ) const {
  bool pass(true);
  if(!info) {
    ATH_MSG_DEBUG( " No MuTagSegmentInfo matchPtDependentPull " );
    return false;
  } 
  if( !info->segment || !info->trackAtSegment ){
    ATH_MSG_DEBUG( " No segment and or trackAtSegment pointermatchPtDependentPull ");
    return false;
  }
  if(!trk) {
    ATH_MSG_DEBUG( " No track pointer " );
    return false;
  }

  double pT( 100. );
  const Trk::Perigee* aMeasPer = trk->perigeeParameters();
  if( aMeasPer ){	
    double sinTheta = sin( aMeasPer->parameters()[Trk::theta] );
    pT = sinTheta * fabs( 1.0/(aMeasPer->parameters()[Trk::qOverP]) ) ;
    pT /= 1000.;
  }

  double Pullcut = 2.0;

  double scale = 1.;
//  if(info->stationLayer==12) scale = 5./3.;
  
  if( pT>2.) {
    Pullcut = 5.0 - 6.0/pT;
  }
  if(Pullcut > scale*m_combinedPullCut ) Pullcut =  scale*m_combinedPullCut;
 
  if(fabs(info->pullCY) > Pullcut) pass = false;
 
  return pass;
}


void MuTagMatchingTool::testExtrapolation( const Trk::Surface* pSurface,
					   const Trk::Track* pTrack
					   ) const {
  
  if( !pSurface ) return;
  if( !pTrack ) return;
  const Trk::Perigee* oriPerigee = pTrack->perigeeParameters();
  if ( oriPerigee == 0 ) {
    ATH_MSG_DEBUG( "Couldn't get the measured Perigee from TP" );
    return;
  }

  // CLHEP::HepVector oripars = oriPerigee->parameters();
  const AmgVector(5)& oripars = oriPerigee->parameters();
  const Trk::PerigeeSurface periSurf ;
  const Trk::Perigee* pPerigee = new Trk::Perigee( oripars[0], oripars[1], oripars[2], oripars[3],0., periSurf, 0) ;
  Amg::Vector3D startPos = pPerigee->position();
  Amg::Vector3D startMom = pPerigee->momentum();
  const AmgVector(5)& pars = pPerigee->parameters();

  ATH_MSG_DEBUG( "==============================================================================" );
  ATH_MSG_DEBUG( "======= INITIAL TRACK PARAMETERS (PERIGEE)" );
  ATH_MSG_DEBUG( "=== parameters are " << pars[0]        << "  " << pars[1]          << " " << pars[2] << "  " << pars[3] << " " << pars[4] );
  ATH_MSG_DEBUG( "=== global position "<< startPos.x()   << "  " << startPos.y()     << "  " << startPos.z() );
  ATH_MSG_DEBUG( "=== global directn  "<< startMom.phi() << "  " << startMom.theta() );

  const Trk::TrackParameters* alongPars( p_IExtrapolator->extrapolate(*pPerigee, *pSurface, Trk::alongMomentum, false, Trk::muon) );

  ATH_MSG_DEBUG( "======= EXTRAPOLATED ALONG MOMENTUM ORIGINAL PERIGEE" );
  if( alongPars ){
    Amg::Vector3D alongPos = alongPars->position();
    Amg::Vector3D  alongMom = alongPars->momentum();
    
    ATH_MSG_DEBUG( "=== global position "          << alongPos.x()   << "  " << alongPos.y() << "  " << alongPos.z() );
    ATH_MSG_DEBUG( "=== global position phi theta "<< alongPos.phi() << "  " << alongPos.theta() );
    ATH_MSG_DEBUG( "=== global directn  "          << alongMom.phi() << "  " << alongMom.theta() );
  } else ATH_MSG_DEBUG( "======= NOT EXTRAPOLATED" );

  ATH_MSG_DEBUG( "======= EXTRAPOLATED OPPOSITE MOMENTUM ORIGINAL PERIGEE" ); 
  const Trk::TrackParameters* oppositePars( p_IExtrapolator->extrapolate(*pPerigee, *pSurface, Trk::oppositeMomentum, false, Trk::muon) );
  if( oppositePars ){
    Amg::Vector3D oppositePos = oppositePars->position();
    Amg::Vector3D oppositeMom = oppositePars->momentum();
    ATH_MSG_DEBUG( "=== global position "          << oppositePos.x()   << "  " << oppositePos.y() << "  " << oppositePos.z() );
    ATH_MSG_DEBUG( "=== global position phi theta "<< oppositePos.phi() << "  " << oppositePos.theta() );
    ATH_MSG_DEBUG( "=== global directn  "          << oppositeMom.phi() << "  " << oppositeMom.theta() );
  }  else ATH_MSG_DEBUG( "======= NOT EXTRAPOLATED" );
  double pi(std::acos(-1));
  double flippedPhi = pars[2] + pi;
  if( flippedPhi > pi) flippedPhi -= 2*pi;
  double flippedTheta = pi - pars[3] ;
  if( flippedTheta < 0. ) flippedTheta += pi;
  const Trk::PerigeeSurface perigSurf ;
  const Trk::Perigee* flippedPerigee = new Trk::Perigee( -pars[0], pars[1], flippedPhi, flippedTheta, pars[4], perigSurf, 0 ) ;
  //CLHEP::HepVector flipPars = flippedPerigee->parameters();
  const AmgVector(5)& flipPars = flippedPerigee->parameters();
  Amg::Vector3D flipPos = flippedPerigee->position();
  Amg::Vector3D flipMom = flippedPerigee->momentum();
  

  ATH_MSG_DEBUG( "======= FLIPPED TRACK PARAMETERS (PERIGEE)" );
  ATH_MSG_DEBUG( "=== phi and theta were " << pars[2]       << "  " << pars[3]     << " and are flipped to " << flippedPhi << "  " << flippedTheta  );
  ATH_MSG_DEBUG( "=== parameters are "     << flipPars[0]   << "  " << flipPars[1] << "  " << flipPars[2] << "  " << flipPars[3] << "  " << flipPars[4] );
  ATH_MSG_DEBUG( "=== global position "    << flipPos.x()   << "  " << flipPos.y() << "  " << flipPos.z() );
  ATH_MSG_DEBUG( "=== global directn  "    << flipMom.phi() << "  " << flipMom.theta() );

  const Trk::TrackParameters* alongFlipPars( p_IExtrapolator->extrapolate(*flippedPerigee, *pSurface, Trk::alongMomentum, false, Trk::muon) );

  ATH_MSG_DEBUG( "======= EXTRAPOLATED ALONGFLIP MOMENTUM ORIGINAL PERIGEE" );
  if( alongFlipPars ) {
    Amg::Vector3D alongFlipPos = alongFlipPars->position();
    Amg::Vector3D alongFlipMom = alongFlipPars->momentum();
    ATH_MSG_DEBUG( "=== global position "          << alongFlipPos.x()   << "  " << alongFlipPos.y() << "  " << alongFlipPos.z() );
    ATH_MSG_DEBUG( "=== global position phi theta "<< alongFlipPos.phi() << "  " << alongFlipPos.theta() );
    ATH_MSG_DEBUG( "=== global directn  "          << alongFlipMom.phi() << "  " << alongFlipMom.theta() );
  } else ATH_MSG_DEBUG( "======= NOT EXTRAPOLATED" );


  ATH_MSG_DEBUG( "======= EXTRAPOLATED OPPOSITEFLIP MOMENTUM ORIGINAL PERIGEE" ); 
  const Trk::TrackParameters* oppositeFlipPars( p_IExtrapolator->extrapolate(*flippedPerigee, *pSurface, Trk::oppositeMomentum, false, Trk::muon) );
  if( oppositeFlipPars ){
    Amg::Vector3D oppositeFlipPos = oppositeFlipPars->position();
    Amg::Vector3D oppositeFlipMom = oppositeFlipPars->momentum();
    ATH_MSG_DEBUG( "=== global position "          << oppositeFlipPos.x()   << "  " << oppositeFlipPos.y() << "  " << oppositeFlipPos.z() );
    ATH_MSG_DEBUG( "=== global position phi theta "<< oppositeFlipPos.phi() << "  " << oppositeFlipPos.theta() );
    ATH_MSG_DEBUG( "=== global directn  "          << oppositeFlipMom.phi() << "  " << oppositeFlipMom.theta() );
  } else ATH_MSG_DEBUG( "======= NOT EXTRAPOLATED" );

  ATH_MSG_DEBUG( "==============================================================================" );
  delete flippedPerigee;
  return;
}

//==========Counts ROTs and CompetingROTs on segment.
void MuTagMatchingTool::nrTriggerHits( const Muon::MuonSegment* seg, int& nRPC, int& nTGC ) const{
  nRPC = 0;
  nTGC = 0;
  std::vector<const Trk::MeasurementBase*> mbs = seg->containedMeasurements();
  //    for( unsigned int i = 0; i< seg->numberOfContainedROTs(); ++i){
  for( unsigned int i = 0; i< mbs.size(); ++i){
    
    const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*> (mbs[i]);
      if (!rot){
      const Trk::CompetingRIOsOnTrack* crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*> (mbs[i]);
      if (crot) rot  = &crot->rioOnTrack(0);
     }	
    if( !rot ) {
      continue;
    }
    if( m_idHelperSvc->isRpc( rot->identify() ) ) ++nRPC;
    if( m_idHelperSvc->isTgc( rot->identify() ) ) ++nTGC;
  }
  
}


MuonCombined::MuonSegmentInfo MuTagMatchingTool::muTagSegmentInfo( const Trk::Track* track, const Muon::MuonSegment*  segment,  const Trk::AtaPlane* exTrack ) const{

   MuonCombined::MuonSegmentInfo info = MuonCombined::MuonSegmentInfo();

//  segment and track pointers
   
   info.track = track;
   info.segment = segment;
   info.trackAtSegment  = exTrack;

   if( !segment ) return info;
   if( !exTrack ) return info;

// hit summaries

   Muon::IMuonSegmentHitSummaryTool::HitCounts hitCounts = m_hitSummaryTool->getHitCounts(*segment);

// quality 

   info.quality = m_selectionTool->quality(*segment);

// Global angular differences
 
   info.dtheta = exTrack->momentum().theta() - segment->globalDirection().theta();
   double dotprod = exTrack->momentum().x() * segment->globalDirection().x() + exTrack->momentum().y() * segment->globalDirection().y();
   dotprod = dotprod/exTrack->momentum().perp();
   if(dotprod>1.) dotprod = 1.;
   if(dotprod<-1.) dotprod = -1.;
   info.dphi = acos(dotprod);

// Global angular differences for position

   info.dthetaPos = exTrack->position().theta() - segment->globalPosition().theta();
   dotprod = cos(exTrack->position().phi()) * cos(segment->globalPosition().phi())  + sin(exTrack->position().phi()) * sin(segment->globalPosition().phi());
   if(dotprod>1.) dotprod = 1.;
   if(dotprod<-1.) dotprod = -1.;
   info.dphiPos = acos(dotprod);


// Local positions    

    info.resY = exTrack->parameters()[Trk::locY] - segment->localParameters()[Trk::locY] ; // delta_theta
    info.resX = exTrack->parameters()[Trk::locX] - segment->localParameters()[Trk::locX] ;  // delta_phi

// Local errors and covariance Matrix

    info.exCovYTheta = 0.;
    // const Trk::TrackParameters* measPars = dynamic_cast<const Trk::TrackParameters*>(exTrack);
    if( exTrack->covariance() ){
      ATH_MSG_DEBUG("Measured Parameters: error x " << (*exTrack->covariance())(Trk::locX,Trk::locX)
		    << " y " << (*exTrack->covariance())(Trk::locY,Trk::locY)
		    << " pull x " <<  info.resX/sqrt((*exTrack->covariance())(Trk::locX,Trk::locX))
		    << " pull y " <<  info.resY/sqrt((*exTrack->covariance())(Trk::locY,Trk::locY)) );
      info.exErrorX = sqrt((*exTrack->covariance())(Trk::locX,Trk::locX));
      info.exErrorY = sqrt((*exTrack->covariance())(Trk::locY,Trk::locY));
      if( (*exTrack->covariance())(Trk::theta,Trk::locY) != 0) {
        info.exCovYTheta = (*exTrack->covariance())(Trk::theta,Trk::locY);
      }
    }else{
      info.exErrorX = -999.;
      info.exErrorY = -999.;
    }

// Segment local errors

    info.segErrorX = sqrt((segment->localCovariance())(Trk::locX,Trk::locX));
    info.segErrorY = sqrt((segment->localCovariance())(Trk::locY,Trk::locY));

// Pull local X and Y
    double erexX =  errorProtection(info.exErrorX, false );
    double erexY =  errorProtection(info.exErrorY, false );
    info.pullX = info.resX / sqrt(erexX*erexX + info.segErrorX*info.segErrorX + m_SAFE_PHI*m_SAFE_PHI);
    info.pullY = info.resY / sqrt(erexY*erexY + info.segErrorY*info.segErrorY + m_SAFE_THETA*m_SAFE_THETA);

// Local angles
     
    Trk::LocalDirection segLocDir =  segment->localDirection() ;
    Trk::LocalDirection exTrkLocDir ;
    exTrack->associatedSurface().globalToLocalDirection(exTrack->momentum(),exTrkLocDir);
    info.dangleYZ = exTrkLocDir.angleYZ() - segLocDir.angleYZ(); //deltaYZ
    info.dangleXZ = exTrkLocDir.angleXZ() - segLocDir.angleXZ(); //deltaXZ
    //delete exTrkLocDir;

// Errors on local angles
    double exTrkErrXZ(0.), exTrkErrYZ(0.), segErrXZ(0.), segErrYZ(0.), covLocYYZ(0.);

    calculateLocalAngleErrors ( exTrack, exTrkErrXZ, exTrkErrYZ, covLocYYZ );
    info.exErrorXZ = exTrkErrXZ;
    info.exErrorYZ = exTrkErrYZ;
    info.exCovYZY = covLocYYZ;
    calculateLocalAngleErrors ( segment, segErrXZ, segErrYZ );
    info.segErrorXZ = segErrXZ;
    info.segErrorYZ = segErrYZ;
    double erexXZ =  errorProtection(info.exErrorXZ, true );
    double erexYZ =  errorProtection(info.exErrorYZ, true );

    ATH_MSG_DEBUG( " exTrkLocDir.angleYZ() " << exTrkLocDir.angleYZ() << " segLocDir.angleYZ() " << segLocDir.angleYZ());  
    ATH_MSG_DEBUG( " exTrkLocDir.angleXZ() " << exTrkLocDir.angleXZ() << " segLocDir.angleXZ() " << segLocDir.angleXZ());  

    ATH_MSG_DEBUG( " info.exErrorYZ  " << info.exErrorYZ  << " info.segErrorYZ " << info.segErrorYZ << " info.exCovYZY " << info.exCovYZY );
    ATH_MSG_DEBUG( " info.exErrorXZ  " << info.exErrorXZ  << " info.segErrorXZ " << info.segErrorXZ );

    info.pullXZ = info.dangleXZ / sqrt(erexXZ*erexXZ + info.segErrorXZ*info.segErrorXZ + m_SAFE_PHIANGLE*m_SAFE_PHIANGLE);
    info.pullYZ = info.dangleYZ / sqrt(erexYZ*erexYZ + info.segErrorYZ*info.segErrorYZ + m_SAFE_THETAANGLE*m_SAFE_THETAANGLE);

   
    ATH_MSG_DEBUG( " info.pullXZ " << info.pullXZ ); 
 

     Identifier chId = m_edmHelperSvc->chamberId(*segment);
     Muon::MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(chId);
//
//  residuals and pulls in X coordinate (along tube)
//

// MuonDetectorManager from the conditions store
     SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
     const MuonGM::MuonDetectorManager* MuonDetMgr = DetectorManagerHandle.cptr(); 
     if(MuonDetMgr==nullptr){
       ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
       return info; 
     } 

     bool first = true;
     double maxResXMdt = -1e9;
     double maxResPhi  = -1e9;
     double maxPullPhi = -1e9;
     double minResPhi  = 1e9;
     double minPullPhi = 1e9;
     // loop over hits
     std::vector<const Trk::MeasurementBase*>::const_iterator mit = segment->containedMeasurements().begin();
     std::vector<const Trk::MeasurementBase*>::const_iterator mit_end = segment->containedMeasurements().end();
     for( ;mit!=mit_end;++mit ){
       
       const Muon::MdtDriftCircleOnTrack* mdt = dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(*mit);
       if( mdt ){
	 
	 // get id and check that it is a muon hit id
	 Identifier id = mdt->identify();
	 
	 // get layer index
	 int lay = m_idHelperSvc->mdtIdHelper().tubeLayer(id);
	 int tube = m_idHelperSvc->mdtIdHelper().tube(id); 
	 
	 const MuonGM::MdtReadoutElement* detEl = mdt->prepRawData() ? mdt->prepRawData()->detectorElement() : MuonDetMgr->getMdtReadoutElement(id);
	 if( !detEl ){
	   ATH_MSG_WARNING(" could not get MdtReadoutElement for tube " << m_idHelperSvc->toString(id));
	   continue;
	 }
	 double tubeLen = detEl->getActiveTubeLength(lay,tube);

	 // use SL within station to speed up extrapolation    
	 const Trk::TrackParameters* exP = p_propagator->propagate(*exTrack, mdt->associatedSurface(), Trk::anyDirection, false, Trk::NoField);
	 if( !exP ){
           ATH_MSG_WARNING("Failed to extrapolate to " << m_idHelperSvc->toString(id));
	   continue;
	 }
	 ATH_MSG_DEBUG(m_idHelperSvc->toString(id) << " exPos " << exP->parameters()[Trk::locR] << " y " << exP->parameters()[Trk::locZ]
		       << " tubeL " << tubeLen);
	 double exResidual = fabs(exP->parameters()[Trk::locZ]) - 0.5*tubeLen;
	 if( maxResXMdt < exResidual ) maxResXMdt = exResidual;
	 if( exResidual > 0. ) ATH_MSG_DEBUG("Extrapolated position outside tube, " << exResidual );
	 delete exP;
       }else{
	 
	 // get id and check that it is a muon hit id
	 Identifier id = m_edmHelperSvc->getIdentifier(**mit);
	 if( !id.is_valid() || !m_idHelperSvc->isMuon(id) ) continue;
	 if( !m_idHelperSvc->measuresPhi(id) ) continue;
	 const Trk::TrackParameters* exP = p_propagator->propagate(*exTrack, (*mit)->associatedSurface(), Trk::anyDirection, false, Trk::NoField);
	 if( !exP ){
	   ATH_MSG_WARNING("Failed to extrapolate to " << m_idHelperSvc->toString(id));
	   continue;
	 }
	 const Trk::ResidualPull* resPull = m_pullCalculator->residualPull( *mit, exP, Trk::ResidualPull::Unbiased );
	 if( !resPull ) {
	   ATH_MSG_WARNING(" calculation of residual/pull failed !!!!! ");
	   delete exP;
	   continue;
	 }
	 double residual = resPull->residual().front(); 
	 double pull = resPull->pull().front();
	 if( first ){
	   first = false;
	   maxResPhi  = residual;
	   maxPullPhi = pull;
	   minResPhi  = residual;
	   minPullPhi = pull;
	 }else{
	   if( fabs(residual) > fabs(maxResPhi) ) maxResPhi  = residual;
	   if( fabs(pull) > fabs(maxPullPhi) )    maxPullPhi = pull;
	   if( fabs(residual) < fabs(minResPhi) ) minResPhi  = residual;
	   if( fabs(pull) > fabs(minPullPhi) )    minPullPhi = pull;
	 }
	 ATH_MSG_DEBUG(m_idHelperSvc->toString(id) << " residual " << residual << " pull " << pull);
	 delete resPull;
	 delete exP;
       }
     }
     ATH_MSG_DEBUG("Residual phi min " << minResPhi << " max " << maxResPhi << " pull min " << minPullPhi << " max " << maxPullPhi
		   << " dist from tube end " << maxResXMdt );

//
//   Store the local X variables
//
     info.maximumResidualAlongTube =  maxResXMdt;
     info.maximumResidualPhi = maxResPhi;
     info.maximumPullPhi = maxPullPhi;

     info.minimumResidualPhi = minResPhi;
     info.minimumPullPhi = minPullPhi;

     info.pullChamber =  maxResXMdt/info.exErrorX;

//  Scale covariance Matrix (if needed)

    double a = info.exErrorY*info.exErrorY;
    double b = info.exCovYZY;
    double d = info.exErrorYZ*info.exErrorYZ;
    double det = a*d-b*b;

    double scale = 1.;
    if(a*d - b*b < 0.1*a*d) {
      scale = sqrt(0.9*a*d)/sqrt(b*b);
      det = a*d-scale*b*scale*b;
    }
    double dydyz = scale*info.exCovYZY;
    double correction = dydyz/(info.exErrorYZ*info.exErrorYZ);

// 
//  residual after taking into account the correlation with the angle YZ
//

    info.resCY =  info.resY - correction*info.dangleYZ;

   ATH_MSG_DEBUG(" segment direction theta " << segment->globalDirection().theta() << " position theta " <<  segment->globalPosition().theta() << " correction " << correction ); 
   ATH_MSG_DEBUG(" resY " << info.resY << " dangleYZ " << info.dangleYZ << " resCY " << info.resCY );
   double error_rescy = info.exErrorY*info.exErrorY -  correction*correction*info.exErrorYZ*info.exErrorYZ;
   double error_segcy = info.segErrorY*info.segErrorY +  correction*correction*info.segErrorYZ*info.segErrorYZ;
   if(error_rescy>0) {
     error_rescy = sqrt(error_rescy+error_segcy);
   } else {
     error_rescy = sqrt(info.exErrorY*info.exErrorY+error_segcy);
   }

//  pull after taking into account the correlation with the angle YZ

    info.pullCY =  info.resCY / sqrt(error_rescy*error_rescy + m_SAFE_THETA*m_SAFE_THETA +  correction*correction*m_SAFE_THETAANGLE*m_SAFE_THETAANGLE );

// chi2 with full covariance matrix in Y and YZ plane 

    double chi2Y = d*info.resY*info.resY - 2*b*scale*info.resY*info.dangleYZ + a*info.dangleYZ*info.dangleYZ;
    info.chi2Y =  chi2Y/det/2.;
    if (info.chi2Y<0) ATH_MSG_DEBUG( " NEGATIVE chi2Y " << chi2Y << " dydyz " << dydyz << " determinant " << det );

     bool hasPhi = false;

    if(hitCounts.nexpectedTrigHitLayers> 1) hasPhi = true; 

//
// Store hasphi
//
     info.hasPhi = 0;
     if(hasPhi) info.hasPhi = 1;
     info.t0 = 0.;

     // station layer 
     info.stationLayer = 0;
     if( stIndex == Muon::MuonStationIndex::BI ) info.stationLayer = 1;
     if( stIndex == Muon::MuonStationIndex::BM ) info.stationLayer = 2;
     if( stIndex == Muon::MuonStationIndex::BO ) info.stationLayer = 3;
     if( stIndex == Muon::MuonStationIndex::BE ) info.stationLayer = 4;
     if( stIndex == Muon::MuonStationIndex::EI ) info.stationLayer = m_idHelperSvc->isMdt(chId) ? 11 : 21;
     if( stIndex == Muon::MuonStationIndex::EM ) info.stationLayer = 12;     
     if( stIndex == Muon::MuonStationIndex::EO ) info.stationLayer = 13;
     if( stIndex == Muon::MuonStationIndex::EE ) info.stationLayer = 14;

     ATH_MSG_DEBUG( "stationLayer " << info.stationLayer );
     // number of holes
     info.nholes = 0;
     const Muon::MuonSegmentQuality* q = dynamic_cast<const  Muon::MuonSegmentQuality*>(segment->fitQuality());
     if( q ) info.nholes = q->numberOfHoles();

     // number of segments
     info.nsegments =1;

     // single ML
     info.singleML = 1;
     if(hitCounts.nmdtHitsMl1>0 && hitCounts.nmdtHitsMl2>0) info.singleML = 0;


//   RLocY parametrized rejection as a function of locY 
//   RAYZ parametrized rejection as a function of Angle YZ 

//   parameters are used for selecting best segment in ambiguity solver 
 
   double a_locY(13.8269), b_locY(1.23548), c_locY(2.73400) ;
   double a_AYZ(12.0655),  b_AYZ(1.87578),  c_AYZ(1.88660);
   double width_locY( 20.) , width_AYZ( 0.004 );

   double dlocY  = info.resY;
   double dAYZ   = info.dangleYZ; 

   double logLocY =  log(1+ fabs( dlocY / width_locY ) ) ;
   double logAYZ  =  log(1+ fabs( dAYZ  / width_AYZ  ) );

   info.RLocY =  a_locY /( 1. + b_locY*( logLocY )*( logLocY ) + c_locY*( logLocY )*( logLocY )*( logLocY ))    ;
   info.RAYZ =  a_AYZ /( 1. + b_AYZ*( logAYZ )*( logAYZ) + c_AYZ*( logAYZ )*( logAYZ )*( logAYZ ))    ;

   int selected = 0;
   scale = 1.;

   if(fabs(info.pullY)<m_MATCH_THETA&&fabs(info.pullYZ)<m_MATCH_THETAANGLE&&fabs(info.pullCY)<scale*m_combinedPullCut&&info.pullChamber<m_chamberPullCut){
     bool pass = true;
     if(pass) selected = 1;
   } 
     

   info.selected = selected;

   return info;
   
 }
 void MuTagMatchingTool::calculateLocalAngleErrors( const Muon::MuonSegment*   segment,  double& angleXZerror, double& angleYZerror ) const {

   const Amg::MatrixX& segGlobCov = segment->localCovariance();
  angleXZerror = 999.;
  angleYZerror = 999.;
//
// the segment errors are stored in local angles (m_assumeLocalErrors = true);
//

  if( m_assumeLocalErrors ) {
    if ( segGlobCov(Trk::phi,Trk::phi)     >= 0 && segGlobCov(Trk::phi,Trk::phi)     <= 999.0 ) angleXZerror = std::sqrt( segGlobCov(Trk::phi,Trk::phi) );
    if ( segGlobCov(Trk::theta,Trk::theta) >= 0 && segGlobCov(Trk::theta,Trk::theta) <= 999.0) angleYZerror = std::sqrt( segGlobCov(Trk::theta,Trk::theta) );
  }

 }

 void  MuTagMatchingTool::calculateLocalAngleErrors( const Trk::AtaPlane* exTrack, double& angleXZerror, double& angleYZerror, double& covLocYYZ ) const {

  // Parameters are described as Trk::LocX, Trk::locY, Trk::phi, Trk::theta
  // So the errormatrix of the track 'localErrorMatrix' still holds global angle representation!!!!
  //retrieve Jabcobian to transform the global errors err_phi,err_theta to local errors err_alphaXZ, err_alphaYZ
   const Amg::RotationMatrix3D& glob2loc = exTrack->associatedSurface().transform().rotation().inverse();
   const AmgVector(5)& exTrkParms = exTrack->parameters();
  Trk::JacobianPhiThetaLocalAngles jacobianExTrk( exTrkParms[Trk::phi], exTrkParms[Trk::theta],  glob2loc );

  // start with global angles error matrix
  Amg::MatrixX anglesCovGlob(2,2);
  anglesCovGlob(0,0) = (*exTrack->covariance())(Trk::phi,Trk::phi);
  anglesCovGlob(1,0) = (*exTrack->covariance())(Trk::theta,Trk::phi);
  anglesCovGlob(0,1) = (*exTrack->covariance())(Trk::phi,Trk::theta);
  anglesCovGlob(1,1) = (*exTrack->covariance())(Trk::theta,Trk::theta);

  // NEW METHOD
  const AmgSymMatrix(2)& anglesCovLoc = anglesCovGlob.similarity(jacobianExTrk);

  if (anglesCovLoc(0,0) >= 0) angleXZerror = std::sqrt( anglesCovLoc(0,0) );
  if (anglesCovLoc(1,1) >= 0) angleYZerror = std::sqrt( anglesCovLoc(1,1) );

  covLocYYZ = (*exTrack->covariance())(Trk::locY,Trk::phi)*jacobianExTrk(0,1) + (*exTrack->covariance())(Trk::locY,Trk::theta)*jacobianExTrk(1,1);
  ATH_MSG_DEBUG(" covLocYYZ " << covLocYYZ << " covlocytheta " << (*exTrack->covariance())(Trk::locY,Trk::theta) << " J01 " << jacobianExTrk(0,1) << " J11 " << jacobianExTrk(0,1));
  // some printout
  ATH_MSG_DEBUG( std::setw(20) << "Angles Jacobian used for TRACK angle errors below: " << jacobianExTrk );

  ATH_MSG_DEBUG( std::setw(20) << "NEW TRACK angleXZ error = " <<
                 std::setprecision(6) << std::setw(10) << angleXZerror <<
                 std::setw(20) << " and angleYZ error = " << std::setw(10) << angleYZerror );

 }
 bool MuTagMatchingTool::matchDistance( MuonCombined::MuonSegmentInfo* info) const {

  bool pass(true);
  if(!info) {
    ATH_MSG_DEBUG( " No MuTagSegmentInfo matchDistance " );
    return false;
  } 
  if( !info->segment || !info->trackAtSegment ){
      ATH_MSG_DEBUG( " No segment and or trackAtSegment pointer matchDistance ");
    return false;
  }
  if( !m_doDistCut ) return pass;

     if( info->hasPhi ){
       if( fabs( info->resX ) > m_DIST_POS      )   pass = false;    
       if( fabs( info->dangleXZ )   > m_DIST_ANGLE )   pass = false;
     }
     if( info->maximumResidualAlongTube > m_DIST_POS )                 pass = false;
     if( fabs( info->resY ) > m_DIST_POS      )   pass = false;    
     if( fabs( info->dangleYZ )     > m_DIST_ANGLE )   pass = false;
    
  return pass;   
 
 }
 
bool MuTagMatchingTool::isCscSegment( const Muon::MuonSegment* seg ) const { 
  bool isCsc(false);

  std::vector<const Trk::MeasurementBase*> mbs = seg->containedMeasurements();
  for( unsigned int i = 0; i< mbs.size(); ++i){
   
    const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*> (mbs[i]);
    if (!rot){
      const Trk::CompetingRIOsOnTrack* crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*> (mbs[i]);
      if (crot) rot  = &crot->rioOnTrack(0);
    }	
    if( !rot ) {
      continue;
    }
    if( m_idHelperSvc->isCsc( rot->identify() ) ) isCsc=true;
  }

  return isCsc;
}

unsigned int MuTagMatchingTool::cscHits( const Muon::MuonSegment* seg ) const {
  unsigned int nrHits(0);
  if( !isCscSegment(seg) ) return nrHits;
  
  std::vector<const Trk::MeasurementBase*> mbs = seg->containedMeasurements();
  for( unsigned int i = 0; i< mbs.size(); ++i){
    
    const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*> (mbs[i]);
    if (!rot){
      const Trk::CompetingRIOsOnTrack* crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*> (mbs[i]);
      if (crot) rot  = &crot->rioOnTrack(0);
    }	
    if( !rot ) {
      continue;
    }
    if( m_idHelperSvc->isCsc( rot->identify() ) ) ++nrHits;
  }
  
  return nrHits ;
}
