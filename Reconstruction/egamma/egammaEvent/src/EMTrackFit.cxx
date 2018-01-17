/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 /********************************************************************

NAME:     EMTrackFit.cxx
PACKAGE:  offline/Reconstruction/egammaEvent

AUTHORS:  Anthony Morley 
CREATED:  July 2008

PURPOSE:  An awful looking object that contains the Electron Track refit information
MODIFIED:
         Jun 10, 2009 (FD/AM) Small changes to include brem fit status, 
                              brem radius uncertainty and fit Chi2
         Jun 28, 1010 (JM) Change float to double in accessor functions
********************************************************************/

// INCLUDE HEADER FILES:

#include <math.h>
#include "egammaEvent/EMTrackFit.h"
#include "GaudiKernel/GaudiException.h"
#include "SGTools/ClassName.h"
#include "TrkTrack/Track.h" 
#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "SGTools/BaseInfo.h"



//  END OF HEADER FILES INCLUDE

// ----------------------------
//  Constructor
// ----------------------------

// give default values
EMTrackFit::EMTrackFit()
  : egDetail()
{ } 

// destructor
EMTrackFit::~EMTrackFit()
{ 
}

// interfaces
std::string EMTrackFit::s_className = ClassName<EMTrackFit>::name();
const std::string& EMTrackFit::className() const
{
  return s_className;
}

// ==============================================================
double EMTrackFit::parameter(egammaParameters::ParamDef key) const 
{
  
  if ( hasIntParameter(key) ) {
    return (double) intParameter(key);
  }
  
  typedef std::pair<egammaParameters::ParamDef,double> elParams;
  
  double result = egammaParameters::EgParamUndefined;
  std::vector<elParams>::const_iterator p = m_parameters.begin();
  
  for (;p !=m_parameters.end(); p++) {
    if ( (*p).first == key ) {
      result = (*p).second;
      break;
    }
  }

  return result;
}

// ===================================================================
void EMTrackFit::set_parameter(egammaParameters::ParamDef key, double value, bool overwrite) {

  if ( hasIntParameter(key) ) {
    set_parameterInt(key,(int)value,overwrite);
  }

  typedef std::pair<egammaParameters::ParamDef,double> elParams;

  std::vector<elParams>::iterator p = m_parameters.begin();
 
  for (;p !=m_parameters.end(); p++) {
    if ( (*p).first == key ) break;
  }

  if ( p == m_parameters.end() ) {
    m_parameters.push_back( elParams(key,value) );
  }
  else {
    if ( overwrite ) {
      (*p).second = value;
    }
    else {      
      throw GaudiException("parameter not saved", "EMTrackFit::set_parameter(...)", StatusCode::FAILURE);
    }
  }

}

// ==================================================================
int EMTrackFit::intParameter(egammaParameters::ParamDef key) const 
{
  typedef std::pair<egammaParameters::ParamDef,int> elParams;

  int result = (int)egammaParameters::EgParamUndefined;
  std::vector<elParams>::const_iterator p = m_parametersInt.begin();
 
  for (;p !=m_parametersInt.end(); p++) {
    if ( (*p).first == key ){
      result = (*p).second;
      break;
    }
  }

  return result;
}

// ===================================================================
void EMTrackFit::set_parameterInt(egammaParameters::ParamDef key, int value, bool overwrite) {

  typedef std::pair<egammaParameters::ParamDef,int> elParams;

  std::vector<elParams>::iterator p = m_parametersInt.begin();
 
  for (;p !=m_parametersInt.end(); p++) {
    if ( (*p).first == key ) break;
  }

  if ( p == m_parametersInt.end() ) {
    m_parametersInt.push_back( elParams(key,value) );
  }
  else {
    if ( overwrite ) {
      (*p).second = value;
    }
    else {      
      throw GaudiException("parameter not saved", "EMTrackFit::set_parameter(...)", StatusCode::FAILURE);
    }
  }

}

// ========================================================================
void EMTrackFit::fillDetails(Trk::Track *track){
  if (track){
    bremTrackAuthor( track->info().trackFitter() );
    fillPerigeeParamters( track->perigeeParameters() );
    fillLastMeasurement(track);
    fillBrems(track);
  } else {
    throw GaudiException("Parameters not saved, no track", "EMTrackFit::fillDetails(...)", StatusCode::FAILURE);
  }
  
  return;
}

// =======================================================================
void EMTrackFit::fillBrems(Trk::Track *track){
  
  std::vector<const Trk::EstimatedBremOnTrack*> estimatedBremOnTrack;
  std::vector<const Trk::TrackStateOnSurface* >  trkStateOnSurfaceWithBrem;
  
  DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStateOnSurface = track->trackStateOnSurfaces()->begin();    
  //For look for all of the EstimatedBremsOnTrack and add the pointers to a vector
  for (;trackStateOnSurface < track->trackStateOnSurfaces()->end(); ++trackStateOnSurface){
    const Trk::EstimatedBremOnTrack* brem = dynamic_cast<const Trk::EstimatedBremOnTrack*>((*trackStateOnSurface)->materialEffectsOnTrack());
    if (brem){
      estimatedBremOnTrack.push_back (  brem );
  	  trkStateOnSurfaceWithBrem.push_back( (*trackStateOnSurface) );
  	}
  }
  
  if (estimatedBremOnTrack.size() == 0){
    hasBrem(0);
    bremRadius(0);
    bremDeltaZ(0);
    return;
  } else {
    // The energy loss weighted average position  of the brem.
    std::vector<const Trk::EstimatedBremOnTrack*>::iterator brems = estimatedBremOnTrack.begin();
    std::vector<const Trk::TrackStateOnSurface*>::iterator tsos = trkStateOnSurfaceWithBrem.begin();
    
    double Z(1.);
    double R(0.);
    double fractionELost(0.);
    double sigmaRetainedEnFraction(0);
    for (; brems!=estimatedBremOnTrack.end(); ++brems, ++tsos) {
      
      fractionELost += 1-(*brems)->retainedEnFraction();
      
      Z *= (*brems)->retainedEnFraction();
      
      R += (1-(*brems)->retainedEnFraction()) * (*tsos)->trackParameters()->position().perp();
      sigmaRetainedEnFraction += (1-(*brems)->retainedEnFraction()) * (*brems)->sigmaRetainedEnFraction();
    }
    if (Z>0 && fractionELost>0.){
      R/=fractionELost;
      sigmaRetainedEnFraction/=fractionELost;
    }
    hasBrem(estimatedBremOnTrack.size());
    bremRadius(R);
    bremDeltaZ(Z);
    //Clearly this is poorly defined for multiple brems a better way of doing this need to be found
		bremDeltaZerr(sigmaRetainedEnFraction);
  }
  return;
}

// ======================================================================
void EMTrackFit::fillLastMeasurement(Trk::Track *track){
  
  if (track == 0 ){
    return;
  }

  /**  
  DataVector< const Trk::TrackParameters >::const_reverse_iterator lastTrkParameters = track->trackParameters()->rbegin();
  DataVector< const Trk::TrackParameters >::const_iterator firstTrkParameters = track->trackParameters()->begin();
    
  if (!(*lastTrkParameters))  return;
  if (!(*firstTrkParameters)) return;
  
  //Check the position of the last surface
  const Trk::GlobalPosition& posOnSurf = (*lastTrkParameters)->position();
  double finalRadius = posOnSurf.perp();

  const Trk::GlobalPosition& firstposOnSurf = (*firstTrkParameters)->position();
  double firstRadius = firstposOnSurf.perp();
  
  //Assuming if that the trejectry has been filled in some kind of order flip 
  if (firstRadius > finalRadius){
    trkPara =  (*firstTrkParameters);   
  } else {
    trkPara =  (*lastTrkParameters);
  }
  **/
  
  const DataVector<const Trk::TrackStateOnSurface>* oldTrackStates = track->trackStateOnSurfaces();
  if (oldTrackStates == 0)
  {
    return;
  }
    
  for ( DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator rItTSoS = oldTrackStates->rbegin(); rItTSoS != oldTrackStates->rend(); ++rItTSoS)
  { 

    if ( (*rItTSoS)->type(Trk::TrackStateOnSurface::Measurement) && (*rItTSoS)->trackParameters()!=0 && (*rItTSoS)->measurementOnTrack()!=0)
    {
      const Trk::TrackParameters* trkPara = (*rItTSoS)->trackParameters();
      track_LastM_loc1     ( trkPara->parameters()[Trk::loc1]);
      track_LastM_loc2     ( trkPara->parameters()[Trk::loc2]);
      track_LastM_phi      ( trkPara->parameters()[Trk::phi]);
      track_LastM_theta    ( trkPara->parameters()[Trk::theta]);
      track_LastM_qOverP   ( trkPara->parameters()[Trk::qOverP]);
      break;
    }
  }
  return;
}

// ========================================================================
bool EMTrackFit::fillPerigeeParamters(const Trk::Perigee *trackParameters){
 if (trackParameters){
   track_perigee_d0(trackParameters->parameters()[Trk::d0]);
   track_perigee_z0(trackParameters->parameters()[Trk::z0]);
   track_perigee_phi0(trackParameters->parameters()[Trk::phi]);
   track_perigee_theta(trackParameters->parameters()[Trk::theta]);
   track_perigee_qOverP(trackParameters->parameters()[Trk::qOverP]);
   
   if ( !(fillPerigeeErrors(trackParameters->covariance())) ){
     // This Failed Jolly good
   }
   
   return true;
  } else {
    return false;
  }
  return true;
}

bool EMTrackFit::fillPerigeeErrors(const AmgSymMatrix(5)* errorMatrix)
{
  if(!errorMatrix)
    return false;
  
  
  track_perigee_Covd0d0         ((*errorMatrix)(Trk::d0,    Trk::d0))     ;
  track_perigee_Covd0z0         ((*errorMatrix)(Trk::d0,    Trk::z0))     ;
  track_perigee_Covd0phi        ((*errorMatrix)(Trk::d0,    Trk::phi))    ;
  track_perigee_Covd0theta      ((*errorMatrix)(Trk::d0,    Trk::theta))  ;
  track_perigee_Covd0qOverP     ((*errorMatrix)(Trk::d0,    Trk::qOverP)) ;
  track_perigee_Covz0z0         ((*errorMatrix)(Trk::z0,    Trk::z0))     ;
  track_perigee_Covz0phi        ((*errorMatrix)(Trk::z0,    Trk::phi))    ;
  track_perigee_Covz0theta      ((*errorMatrix)(Trk::z0,    Trk::theta))  ;
  track_perigee_Covz0qOverP     ((*errorMatrix)(Trk::z0,    Trk::qOverP)) ;
  track_perigee_Covphiphi       ((*errorMatrix)(Trk::phi,   Trk::phi))    ;
  track_perigee_Covphitheta     ((*errorMatrix)(Trk::phi,   Trk::theta))  ;
  track_perigee_CovphiqOverP    ((*errorMatrix)(Trk::phi,   Trk::qOverP)) ;
  track_perigee_Covthetatheta   ((*errorMatrix)(Trk::theta, Trk::theta))  ;
  track_perigee_CovthetaqOverP  ((*errorMatrix)(Trk::theta, Trk::qOverP)) ;
  track_perigee_CovqOverPqOverP ((*errorMatrix)(Trk::qOverP,Trk::qOverP)) ;
  return true;
}

// =======================================================================
AmgSymMatrix(5)* EMTrackFit::getErrorMatrix() const
{
  // Make a 5x5 matrix that is filled with zero's alternatively we could fill it with the identity matrix;
  // Chosen the identity matrix option
  AmgSymMatrix(5)* hepSymMatrix  = new AmgSymMatrix(5);
  hepSymMatrix->setIdentity();
  // Fill the matrix
  hepSymMatrix->fillSymmetric(0,0, track_perigee_Covd0d0());
  hepSymMatrix->fillSymmetric(0,1, track_perigee_Covd0z0());
  hepSymMatrix->fillSymmetric(0,2, track_perigee_Covd0phi());
  hepSymMatrix->fillSymmetric(0,3, track_perigee_Covd0theta());
  hepSymMatrix->fillSymmetric(0,4, track_perigee_Covd0qOverP());
  hepSymMatrix->fillSymmetric(1,1, track_perigee_Covz0z0());
  hepSymMatrix->fillSymmetric(1,2, track_perigee_Covz0phi());
  hepSymMatrix->fillSymmetric(1,3, track_perigee_Covz0theta());
  hepSymMatrix->fillSymmetric(1,4, track_perigee_Covz0qOverP());
  hepSymMatrix->fillSymmetric(2,2, track_perigee_Covphiphi());
  hepSymMatrix->fillSymmetric(2,3, track_perigee_Covphitheta());
  hepSymMatrix->fillSymmetric(2,4, track_perigee_CovphiqOverP());
  hepSymMatrix->fillSymmetric(3,3, track_perigee_Covthetatheta());
  hepSymMatrix->fillSymmetric(3,4, track_perigee_CovthetaqOverP());
  hepSymMatrix->fillSymmetric(4,4, track_perigee_CovqOverPqOverP());
  
 
  return hepSymMatrix;
  
}

// ================================================================
const Trk::Perigee* EMTrackFit::getMeasuredPerigee () const
{
  // Create a MeasuredPerigee from the individual doubles stored in the class.
  
  const Trk::PerigeeSurface   surface;
  
  const Trk::Perigee* measuredPerigee = surface.createParameters<5,Trk::Charged>( 
                                                            track_perigee_d0(),
                                                            track_perigee_z0(),
                                                            track_perigee_phi0(),
                                                            track_perigee_theta(),
                                                            track_perigee_qOverP(),
                                                            getErrorMatrix() );
                                        
  return measuredPerigee;

}

// =====================================================================
bool EMTrackFit::hasParameter(egammaParameters::ParamDef key) const {
  if ( hasIntParameter(key) )                                   return true;
  else if ( key == egammaParameters::refittedTrack_d0 )         return true;
  else if ( key == egammaParameters::refittedTrack_phi0 )       return true;
  else if ( key == egammaParameters::refittedTrack_qOverP )     return true;
  else if ( key == egammaParameters::refittedTrack_z0 )         return true;
  else if ( key == egammaParameters::refittedTrack_theta )      return true;
  else if ( key == egammaParameters::refittedTrack_Covd0d0 )    return true;
  else if ( key == egammaParameters::refittedTrack_Covd0z0 )    return true;
  else if ( key == egammaParameters::refittedTrack_Covd0phi )   return true;
  else if ( key == egammaParameters::refittedTrack_Covd0theta ) return true;
  else if ( key == egammaParameters::refittedTrack_Covd0qOverP) return true;
  else if ( key == egammaParameters::refittedTrack_Covz0z0 )    return true;
  else if ( key == egammaParameters::refittedTrack_Covz0phi )   return true;
  else if ( key == egammaParameters::refittedTrack_Covz0theta ) return true;
  else if ( key == egammaParameters::refittedTrack_Covz0qOverP )return true;
  else if ( key == egammaParameters::refittedTrack_Covphiphi )  return true;
  else if ( key == egammaParameters::refittedTrack_Covphitheta )return true;
  else if ( key == egammaParameters::refittedTrack_CovphiqOverP)return true;
  else if ( key == egammaParameters::refittedTrack_Covthetatheta )return true;
  else if ( key == egammaParameters::refittedTrack_CovthetaqOverP)return true;
  else if ( key == egammaParameters::refittedTrack_CovqOverPqOverP)return true;
  else if ( key == egammaParameters::refittedTrackLastM_loc1 )  return true;
  else if ( key == egammaParameters::refittedTrackLastM_loc2 )  return true;
  else if ( key == egammaParameters::refittedTrackLastM_phi )   return true;
  else if ( key == egammaParameters::refittedTrackLastM_theta ) return true;
  else if ( key == egammaParameters::refittedTrackLastM_qOverP )return true;
  else if ( key == egammaParameters::bremDeltaQoverP )          return true;
  else if ( key == egammaParameters::bremDeltaQoverPErr )       return true;
  else if ( key == egammaParameters::bremMaterialTraversed )    return true;
  else if ( key == egammaParameters::bremRadius )               return true;
  else if ( key == egammaParameters::bremClusterRadius )        return true;
  else if ( key == egammaParameters::bremFitChi2 )              return true;
  return false;           
}    

// =====================================================================
bool EMTrackFit::hasIntParameter(egammaParameters::ParamDef key) const {
  if (key == egammaParameters::hasBrem)                       return true;
  else if (key == egammaParameters::bremTrackAuthor )         return true;
  else if (key == egammaParameters::bremFitStatus )         return true;
  else if (key == egammaParameters::linkIndex )               return true;
  
  return false;
}

// =======================================================================
double EMTrackFit::track_perigee_d0    ()  const {return parameter(egammaParameters::refittedTrack_d0)  ;}
double EMTrackFit::track_perigee_phi0  ()  const {return parameter(egammaParameters::refittedTrack_phi0)  ;}
double EMTrackFit::track_perigee_qOverP()  const {return parameter(egammaParameters::refittedTrack_qOverP)  ;}
double EMTrackFit::track_perigee_z0    ()  const {return parameter(egammaParameters::refittedTrack_z0)  ;}
double EMTrackFit::track_perigee_theta ()  const {return parameter(egammaParameters::refittedTrack_theta)  ;}

double EMTrackFit::track_perigee_Covd0d0         () const {return parameter(egammaParameters::refittedTrack_Covd0d0)  ;}
double EMTrackFit::track_perigee_Covd0z0         () const {return parameter(egammaParameters::refittedTrack_Covd0z0)  ;}
double EMTrackFit::track_perigee_Covd0phi        () const {return parameter(egammaParameters::refittedTrack_Covd0phi)  ;}
double EMTrackFit::track_perigee_Covd0theta      () const {return parameter(egammaParameters::refittedTrack_Covd0theta)  ;}
double EMTrackFit::track_perigee_Covd0qOverP     () const {return parameter(egammaParameters::refittedTrack_Covd0qOverP)  ;}
double EMTrackFit::track_perigee_Covz0z0         () const {return parameter(egammaParameters::refittedTrack_Covz0z0)  ;}
double EMTrackFit::track_perigee_Covz0phi        () const {return parameter(egammaParameters::refittedTrack_Covz0phi)  ;}
double EMTrackFit::track_perigee_Covz0theta      () const {return parameter(egammaParameters::refittedTrack_Covz0theta)  ;}
double EMTrackFit::track_perigee_Covz0qOverP     () const {return parameter(egammaParameters::refittedTrack_Covz0qOverP)  ;}
double EMTrackFit::track_perigee_Covphiphi       () const {return parameter(egammaParameters::refittedTrack_Covphiphi)  ;}
double EMTrackFit::track_perigee_Covphitheta     () const {return parameter(egammaParameters::refittedTrack_Covphitheta)  ;}
double EMTrackFit::track_perigee_CovphiqOverP    () const {return parameter(egammaParameters::refittedTrack_CovphiqOverP)  ;}
double EMTrackFit::track_perigee_Covthetatheta   () const {return parameter(egammaParameters::refittedTrack_Covthetatheta)  ;}
double EMTrackFit::track_perigee_CovthetaqOverP  () const {return parameter(egammaParameters::refittedTrack_CovthetaqOverP)  ;}
double EMTrackFit::track_perigee_CovqOverPqOverP () const {return parameter(egammaParameters::refittedTrack_CovqOverPqOverP)  ;}

double EMTrackFit::track_LastM_loc1    ()  const {return parameter(egammaParameters::refittedTrackLastM_loc1)  ;}
double EMTrackFit::track_LastM_loc2    ()  const {return parameter(egammaParameters::refittedTrackLastM_loc2)  ;}
double EMTrackFit::track_LastM_phi     ()  const {return parameter(egammaParameters::refittedTrackLastM_phi)  ;}
double EMTrackFit::track_LastM_theta   ()  const {return parameter(egammaParameters::refittedTrackLastM_theta)  ;}
double EMTrackFit::track_LastM_qOverP  ()  const {return parameter(egammaParameters::refittedTrackLastM_qOverP)  ;}

void EMTrackFit::track_perigee_d0    (double x)  { set_parameter(egammaParameters::refittedTrack_d0,                        x, true); }
void EMTrackFit::track_perigee_phi0  (double x)  { set_parameter(egammaParameters::refittedTrack_phi0,                      x, true); }
void EMTrackFit::track_perigee_qOverP(double x)  { set_parameter(egammaParameters::refittedTrack_qOverP,                    x, true); }
void EMTrackFit::track_perigee_z0    (double x)  { set_parameter(egammaParameters::refittedTrack_z0,                        x, true); }
void EMTrackFit::track_perigee_theta (double x)  { set_parameter(egammaParameters::refittedTrack_theta,                     x, true); }

void EMTrackFit::track_perigee_Covd0d0         (double x)  { set_parameter(egammaParameters::refittedTrack_Covd0d0,         x, true); }
void EMTrackFit::track_perigee_Covd0z0         (double x)  { set_parameter(egammaParameters::refittedTrack_Covd0z0,         x, true); }
void EMTrackFit::track_perigee_Covd0phi        (double x)  { set_parameter(egammaParameters::refittedTrack_Covd0phi,        x, true); }
void EMTrackFit::track_perigee_Covd0theta      (double x)  { set_parameter(egammaParameters::refittedTrack_Covd0theta,      x, true); }
void EMTrackFit::track_perigee_Covd0qOverP     (double x)  { set_parameter(egammaParameters::refittedTrack_Covd0qOverP,     x, true); }
void EMTrackFit::track_perigee_Covz0z0         (double x)  { set_parameter(egammaParameters::refittedTrack_Covz0z0,         x, true); }
void EMTrackFit::track_perigee_Covz0phi        (double x)  { set_parameter(egammaParameters::refittedTrack_Covz0phi,        x, true); }
void EMTrackFit::track_perigee_Covz0theta      (double x)  { set_parameter(egammaParameters::refittedTrack_Covz0theta,      x, true); }
void EMTrackFit::track_perigee_Covz0qOverP     (double x)  { set_parameter(egammaParameters::refittedTrack_Covz0qOverP,     x, true); }
void EMTrackFit::track_perigee_Covphiphi       (double x)  { set_parameter(egammaParameters::refittedTrack_Covphiphi,       x, true); }
void EMTrackFit::track_perigee_Covphitheta     (double x)  { set_parameter(egammaParameters::refittedTrack_Covphitheta,     x, true); }
void EMTrackFit::track_perigee_CovphiqOverP    (double x)  { set_parameter(egammaParameters::refittedTrack_CovphiqOverP,    x, true); }
void EMTrackFit::track_perigee_Covthetatheta   (double x)  { set_parameter(egammaParameters::refittedTrack_Covthetatheta,   x, true); }
void EMTrackFit::track_perigee_CovthetaqOverP  (double x)  { set_parameter(egammaParameters::refittedTrack_CovthetaqOverP,  x, true); }
void EMTrackFit::track_perigee_CovqOverPqOverP (double x)  { set_parameter(egammaParameters::refittedTrack_CovqOverPqOverP, x, true); }

void EMTrackFit::track_LastM_loc1    (double x)  { set_parameter(egammaParameters::refittedTrackLastM_loc1,   x, true); }
void EMTrackFit::track_LastM_loc2    (double x)  { set_parameter(egammaParameters::refittedTrackLastM_loc2,   x, true); }
void EMTrackFit::track_LastM_phi     (double x)  { set_parameter(egammaParameters::refittedTrackLastM_phi,    x, true); }
void EMTrackFit::track_LastM_theta   (double x)  { set_parameter(egammaParameters::refittedTrackLastM_theta,  x, true); }
void EMTrackFit::track_LastM_qOverP  (double x)  { set_parameter(egammaParameters::refittedTrackLastM_qOverP, x, true); }

int EMTrackFit::bremTrackAuthor () const {  return intParameter(egammaParameters::bremTrackAuthor     ); }

int   EMTrackFit::hasBrem         () const { return intParameter(egammaParameters::hasBrem            ); }
double EMTrackFit::bremRadius      () const { return parameter(egammaParameters::bremRadius            ); }
double EMTrackFit::bremRadiusErr   () const { return parameter(egammaParameters::bremClusterRadius     ); }
double EMTrackFit::bremDeltaZ      () const { return parameter(egammaParameters::bremDeltaQoverP       ); }
double EMTrackFit::bremDeltaZerr   () const { return parameter(egammaParameters::bremDeltaQoverPErr    ); }
double EMTrackFit::bremMaterialTraversed () const { return parameter(egammaParameters::bremMaterialTraversed); }

void EMTrackFit::bremTrackAuthor  (int   x){ set_parameterInt(egammaParameters::bremTrackAuthor, x, true); }
void EMTrackFit::hasBrem          (int   x){ set_parameterInt(egammaParameters::hasBrem,         x, true); }
void EMTrackFit::bremRadius       (double x){ set_parameter(egammaParameters::bremRadius,         x, true); }
void EMTrackFit::bremRadiusErr    (double x){ set_parameter(egammaParameters::bremClusterRadius,  x, true); }
void EMTrackFit::bremDeltaZ       (double x){ set_parameter(egammaParameters::bremDeltaQoverP,    x, true); }
void EMTrackFit::bremDeltaZerr    (double x){ set_parameter(egammaParameters::bremDeltaQoverPErr, x, true); }
void EMTrackFit::bremMaterialTraversed (double x){ set_parameter(egammaParameters::bremMaterialTraversed,  x, true); }

void EMTrackFit::bremFitChi2      (double x){ set_parameter(egammaParameters::bremFitChi2,        x, true); }
double EMTrackFit::bremFitChi2     () const { return parameter(egammaParameters::bremFitChi2             ); }

int   EMTrackFit::bremFitStatus() const { return intParameter(egammaParameters::bremFitStatus); }
void  EMTrackFit::bremFitStatus(int x)  { set_parameterInt(egammaParameters::bremFitStatus, x, true)  ;}
  
int   EMTrackFit::linkIndex() const   { return intParameter(egammaParameters::linkIndex); }
void  EMTrackFit::set_linkIndex(int x){ set_parameterInt(egammaParameters::linkIndex,x, true); }


SG_ADD_BASE (EMTrackFit,    egDetail);
