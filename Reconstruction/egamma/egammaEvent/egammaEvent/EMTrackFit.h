/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_EMTRACKFIT_H
#define EGAMMAEVENT_EMTRACKFIT_H

/**
   @class EMTrackFit
          Contains the fit result with addtional brem information
    EMTrackFit is made by EMTrkRefitter.
    It is responsible for bremsstrahlung recovery using inner detector track refitting. It re-calculates track pT accounting for energy of the photons emmitted in a hard bremsstrahlung. 
   @author Anthony Morley
*/

/********************************************************************
NAME:     EMTrackFit.h
PACKAGE:  offline/Reconstruction/egammaEvent
Created   July 2008 by Anthony Morley 
MODIFIED:
         Jun 10, 2009 (FD/AM) Small changes to include brem fit status, 
                              brem radius uncertainty and fit Chi2
 
********************************************************************/

#include <vector>
#include "egammaEvent/egDetail.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk{
  class  Track;
}

class EMTrackFit : public egDetail
{
 public:

  /** @brief Default constructor*/
  EMTrackFit(); 

  /** @brief Destructor*/
  virtual ~EMTrackFit();

  //void print() const; 
  virtual const std::string& className() const;
  
  /** @brief */
  virtual int    intParameter(egammaParameters::ParamDef) const;
  /** @brief */
  virtual double parameter(egammaParameters::ParamDef) const ;
  /** @brief */
  virtual bool   hasParameter(egammaParameters::ParamDef) const ;
  /** @brief checks if the parameter is integer */
  virtual bool   hasIntParameter(egammaParameters::ParamDef) const ;
  /** @brief general set method for parameters */
  void set_parameter(egammaParameters::ParamDef, double, bool overwrite=false) ;
  
  /** @brief Fill the details of the object from a Trk::Track*/
  void fillDetails(Trk::Track *track);
  
  /** @brief returns the MeasuredPerigee from the stored egammaParameters*/
  const Trk::Perigee* getMeasuredPerigee() const;

 /***********************************************************************/ 
 /*                             Get Parameters                          */
 /***********************************************************************/ 
 
  /** @brief transverse impact parameter (distance of closest approach) after Track Refit */
  double track_perigee_d0    ()  const;
  /** @brief azimuth angle of the momentum at the point of closest approach after Track Refit */
  double track_perigee_phi0  ()  const;
  /** @brief 1/pT estimate according to Track Refit */
  double track_perigee_qOverP()  const;
  /** @brief the z value at the point of closest approach after Track Refit */
  double track_perigee_z0    ()  const;
  /** @brief theta of the track after EMTrackFit */
  double track_perigee_theta ()  const;
  
  /** @brief Covariance matrix item (\f$ \d_0,\d_0 \f$) */
  double track_perigee_Covd0d0         () const;
  /** @brief Covariance matrix item (\f$ \d_0,\z_0 \f$) */
  double track_perigee_Covd0z0         () const;
  /** @brief Covariance matrix item (\f$ \d_0,\phi_0 \f$) */
  double track_perigee_Covd0phi        () const;
  /** @brief Covariance matrix item (d0,theta) */
  double track_perigee_Covd0theta      () const;
  /** @brief Covariance matrix item (d0,q/p) */
  double track_perigee_Covd0qOverP     () const;
  /** @brief Covariance matrix item (z0,z0) */
  double track_perigee_Covz0z0         () const;
  /** @brief Covariance matrix item (z0,phi) */
  double track_perigee_Covz0phi        () const;
  /** @brief Covariance matrix item (z0,theta) */
  double track_perigee_Covz0theta      () const;
  /** @brief Covariance matrix item (z0,q/p) */
  double track_perigee_Covz0qOverP     () const;
  /** @brief Covariance matrix item (phi,phi) */
  double track_perigee_Covphiphi       () const;
  /** @brief Covariance matrix item (phi,theta) */
  double track_perigee_Covphitheta     () const;
  /** @brief Covariance matrix item (phi,q/P) */
  double track_perigee_CovphiqOverP    () const;
  /** @brief Covariance matrix item (theta,theta) */
  double track_perigee_Covthetatheta   () const;
  /** @brief Covariance matrix item (theta,q/P) */
  double track_perigee_CovthetaqOverP  () const;
  /** @brief Covariance matrix item (q/P,q/P) */
  double track_perigee_CovqOverPqOverP () const;
    
  /** @brief Last Measurment Surface Local Position 1 after Track Refit */
  double track_LastM_loc1    ()  const;
  /** @brief Last Measurment Surface Local Position 2 after Track Refit */
  double track_LastM_loc2    ()  const;
  /** @brief Last Measurment Surface azimuth angle after Track Refit */
  double track_LastM_phi     ()  const;
  /** @brief Last Measurment Surface theta  after Track Refit */
  double track_LastM_theta   ()  const;
  /** @brief Last Measurment Surface q/pT estimate according to Track Refit */
  double track_LastM_qOverP  ()  const;    
  /** @brief Track Author enum as Defined in Track/Track.h. N.B. 50 for brem track fit*/
  int bremTrackAuthor   () const;
  
  /** @brief The Number of brems identified by the track fitter*/
  int hasBrem           ()  const;
  /** @brief estimated brem radius from EMTrackFit (mm) [x-y plane] (weighted average if more than one brem)*/
  double bremRadius      ()  const;
  /** @brief estimated brem radius uncertainty*/
  double bremRadiusErr   ()  const;
  /** @brief Fraction of the measured perigee momentum retained after brem */
  double bremDeltaZ      ()  const;
  /** @brief Error on the fraction of the energy lost after the brem*/
  double bremDeltaZerr   ()  const;
  /** @brief Amount of Material seen by the particle according to the track fitter (in X0) */
  double bremMaterialTraversed   ()  const;
  /** Fit Chi2/nDoF */
  double bremFitChi2()  const;
  /** @brief Bremfit Status 1 Reached Iteration Limit, 0 Converged, -1 recheached iteration limit, -2 other*/
  int bremFitStatus   () const;

  /** @brief Index of track this EMTrackFit is associated with*/
  int linkIndex         ()  const;

 /***********************************************************************/ 
 /*                             SET PARAMATERS                          */
 /***********************************************************************/ 
 
  
  /** @brief Set Track Author enum as Defined in Track/Track.h*/
  void bremTrackAuthor (int);
  /** @brief The Number of brems identified by the track fitter*/
  void hasBrem        (int) ;
  /** @brief estimated brem radius from EMTrackFit (mm) [x-y plane] (weighted average if more than one brem)*/
  void bremRadius     (double);
  /** @brief estimated brem radius uncertainty*/
  void bremRadiusErr  (double);
  /** @brief Fraction of the measured perigee momentum lost */
  void bremDeltaZ     (double);
  /** @brief Error on the Fraction of the measured perigee momentum lost */
  void bremDeltaZerr  (double);
  /** @brief Set the amount of material seen by the particle according to the track fitter (in X0) */
  void bremMaterialTraversed  (double);

  
  

  /** @brief Set the transverse impact parameter (distance of closest approach) after Track Refit */
  void track_perigee_d0    (double)  ;
  /** @brief Set the azimuth angle of the momentum at the point of closest approach after Track Refit */
  void track_perigee_phi0  (double)  ;
  /** @brief Set the 1/pT estimate according to Track Refit */
  void track_perigee_qOverP(double)  ;
  /** @brief Set the the z value at the point of closest approach after Track Refit */
  void track_perigee_z0    (double)  ;
  /** @brief Set the theta of the track after EMTrackFit */
  void track_perigee_theta (double)  ;
  
  /** @brief Set the Covariance matrix item (d0,d0) */
  void track_perigee_Covd0d0         (double) ;
  /** @brief Set the Covariance matrix item (d0,z0) */
  void track_perigee_Covd0z0         (double) ;
  /** @brief Set the Covariance matrix item (d0,phi) */
  void track_perigee_Covd0phi        (double) ;
  /** @brief Set the Covariance matrix item (d0,theta) */
  void track_perigee_Covd0theta      (double) ;
  /** @brief Set the Covariance matrix item (d0,q/p) */
  void track_perigee_Covd0qOverP     (double) ;
  /** @brief Set the Covariance matrix item (z0,z0) */
  void track_perigee_Covz0z0         (double) ;
  /** @brief Set the Covariance matrix item (z0,phi) */
  void track_perigee_Covz0phi        (double) ;
  /** @brief Set the Covariance matrix item (z0,theta) */
  void track_perigee_Covz0theta      (double) ;
  /** @brief Set the Covariance matrix item (z0,q/p) */
  void track_perigee_Covz0qOverP     (double) ;
  /** @brief Set the Covariance matrix item (phi,phi) */
  void track_perigee_Covphiphi       (double) ;
  /** @brief Set the Covariance matrix item (phi,theta) */
  void track_perigee_Covphitheta     (double) ;
  /** @brief Set the Covariance matrix item (phi,q/P) */
  void track_perigee_CovphiqOverP    (double) ;
  /** @brief Set the Covariance matrix item (theta,theta) */
  void track_perigee_Covthetatheta   (double) ;
  /** @brief Set the Covariance matrix item (theta,q/P) */
  void track_perigee_CovthetaqOverP  (double) ;
  /** @brief Set the Covariance matrix item (q/P,q/P) */
  void track_perigee_CovqOverPqOverP (double) ; 
    
  /** @brief Set the Last Measurment Surface Local Position 1 after Track Refit */
  void track_LastM_loc1    (double)  ;
  /** @brief Set the Last Measurment Surface Local Position 2 after Track Refit */
  void track_LastM_loc2    (double)  ;
  /** @brief Set the Last Measurment Surface azimuth angle after Track Refit */
  void track_LastM_phi     (double)  ;
  /** @brief Set the Last Measurment Surface theta  after Track Refit */
  void track_LastM_theta   (double)  ;
  /** @brief Set the Last Measurment Surface q/pT estimate according to Track Refit */
  void track_LastM_qOverP  (double)  ;

  /** @brief Set the Chi2/nDoF of the fit*/
  void bremFitChi2         (double);
  /** @brief Set Bremfit Status 0 Converged, -1 recheached iteration limit, -2 other*/
  void bremFitStatus       (int);

  /** @brief Index of track this EMTrackFit is associated with*/
  void set_linkIndex       (int) ;


private:
   /** @brief Fill the perigree parameter details */
  bool fillPerigeeParamters(const Trk::Perigee *trackParameters);
  /** @brief Fill the perigree error details */
  bool fillPerigeeErrors(const AmgSymMatrix(5)* );
  /** @brief Fill in brem information from a Trk::Track */
  void fillBrems(Trk::Track *track);
  /** @brief Obtains the last measurment on the Track and fills details into the object*/
  void fillLastMeasurement(Trk::Track *track);
  /** @brief Returns the ErrorMatrix of the perigee of the associated track. 
      If there is no associated track the error matrix will be 0*/
  AmgSymMatrix(5)* getErrorMatrix() const; 

 void set_parameterInt(egammaParameters::ParamDef, int, bool overwrite=false) ; 
 
 std::vector< std::pair<egammaParameters::ParamDef,double> > m_parameters; 
 std::vector< std::pair<egammaParameters::ParamDef,int> > m_parametersInt; 
 static const std::string s_className;
   
};

#endif 


