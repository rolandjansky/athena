/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_EMERRORBUILDER_H
#define EGAMMAEVENT_EMERRORBUILDER_H
/********************************************************************
NAME:     EMErrorDetail.h
PACKAGE:  offline/Reconstruction/egamma/egammaEvent
AUTHORS:  T. Cuhadar Donszelmann
CREATED:  May 2009
PURPOSE:  Build track/conv. tracks/photon error matrices
UPDATED:
********************************************************************/
#include <vector>
//#include "GaudiKernel/ToolHandle.h"
#include "egammaEvent/egDetail.h"
#include "egammaEvent/egamma.h"
#include "TrkParametersBase/ParametersT.h"
#include "VxVertex/RecVertex.h"

class EMClusterErrorsParametrizations;

namespace Trk{
  template<int DIM, class T, class S> class ParametersT;
  class  Track;
}

class EMErrorDetail : public egDetail
{
 public:

  /** @brief Default constructor*/
  EMErrorDetail(); 

  /** @brief Destructor*/
  virtual ~EMErrorDetail();

  //void print() const; 
  virtual const std::string& className() const;
  
  /** @brief */
  virtual double parameter(egammaParameters::ParamDef) const ;
  /** @brief */
  virtual int intParameter(egammaParameters::ParamDef) const ;
  /** @brief */
  virtual bool   hasParameter(egammaParameters::ParamDef) const ;
  /** @brief */
  virtual bool   hasIntParameter(egammaParameters::ParamDef) const ;
  /** @brief general set method for parameters */
  void set_parameter(egammaParameters::ParamDef, double, bool overwrite=false) ; 
  
  // obsolete @brief Fill the details of the object. This is the main function for building the detail*/
  //template <int DIM, class T, class S> void fillEMErrorDetail(const egamma*, const Trk::ParametersT< DIM, T, S>*, 
  //                                                            const EMClusterErrorsParametrizations*, 
  //							      const Amg::Vector3D&);
  
  // obsolete: Fill and get the track covariance matrix from the method where track and cluster information are combined*/
  //bool fillEMTrackCombinedErrorMatrix(const AmgSymMatrix(5)& combinedMatrix);

  /** @brief E/P combined error matrix, natural size */
  Amg::MatrixX getCombinedErrorMatrix() const;
  /** @brief E/P combined error matrix, natural size */
  Amg::MatrixX getUncombinedErrorMatrix() const;

  /** @brief E/P combined error matrix, (E,eta,phi,M) representation */
  AmgSymMatrix(4) get4x4CombinedErrorMatrix() const;
  /** @brief E/P combined error matrix, (E,eta,phi,M) representation */
  AmgSymMatrix(4) get4x4UncombinedErrorMatrix() const;

  /*** THE 6 FUNCTIONS BELOW ARE DEPRICATED FOR EXTERNAL USE
       THEY ARE CALLED BY THE ABOVE FUNCTIONS ***/

  /** @brief E/P combined 5x5 matrix--Depricated for public use */
  AmgSymMatrix(5) getEMTrackCombinedErrorMatrix() const;
  /** @brief Returns the ErrorMatrix of photons--Depricated for public use; */
  AmgSymMatrix(3) getEMPhotonErrorMatrix() const;
  /** @brief Returns the ErrorMatrix of the perigee of the associated track.
      If there is no associated track the error matrix will be 0.
      Depricated for public use */
  AmgSymMatrix(5) getEMTrackUncombinedErrorMatrix() const;

  /** @brief get E/P combined 4x4 matrix in (E,eta,phi,M) representation--depricated for public use */
  AmgSymMatrix(4) get4x4EMTrackCombinedErrorMatrix() const;
  /** @brief get E/P uncombined 4x4 matrix in (E,eta,phi,M) representation--depricated for public use  */
  AmgSymMatrix(4) get4x4EMTrackUncombinedErrorMatrix() const;
  /** @brief get photon 4x4 matrix in (E,eta,phi,M) representation--Depricated for public use; */
  AmgSymMatrix(4) get4x4EMPhotonErrorMatrix() const;


  /****************************************************************/
  /* Converted photon related parameters  :   Get Parameters*/
  /****************************************************************/
  
  double EMconvertedphoton_perigee_d0 ()  const;
  double EMconvertedphoton_perigee_z0 ()  const;
  double EMconvertedphoton_perigee_phi0 ()  const; 
  double EMconvertedphoton_perigee_theta ()  const;
//   double EMconvertedphoton_perigee_Eclus ()  const;
  double EMconvertedphoton_perigee_eta ()  const;
  double EMconvertedphoton_perigee_momentum ()  const; 

  double EMconvertedphoton_perigee_Covd0d0 ()  const;    
  double EMconvertedphoton_perigee_Covd0z0 ()  const;    
  double EMconvertedphoton_perigee_Covd0phi ()  const;  
  double EMconvertedphoton_perigee_Covd0theta ()  const;
//   double EMconvertedphoton_perigee_Covd0Eclus ()  const;
  double EMconvertedphoton_perigee_Covz0z0   ()  const;
  double EMconvertedphoton_perigee_Covz0phi  ()  const;
  double EMconvertedphoton_perigee_Covz0theta ()  const;
//   double EMconvertedphoton_perigee_Covz0Eclus ()  const;
  double EMconvertedphoton_perigee_Covphiphi ()  const;
  double EMconvertedphoton_perigee_Covphitheta ()  const;
//   double EMconvertedphoton_perigee_CovphiEclus ()  const;
  double EMconvertedphoton_perigee_Covthetatheta ()  const;
//   double EMconvertedphoton_perigee_CovthetaEclus ()  const;
//   double EMconvertedphoton_perigee_CovEclusEclus ()  const;


  /****************************************************************/
  /* Converted photon related parameters  :   Set Parameters*/
  /****************************************************************/

  void EMconvertedphoton_perigee_d0 (double);
  void EMconvertedphoton_perigee_z0 (double);
  void EMconvertedphoton_perigee_phi0 (double); 
  void EMconvertedphoton_perigee_theta (double);
//   void EMconvertedphoton_perigee_Eclus (double);
  void EMconvertedphoton_perigee_eta (double);
  void EMconvertedphoton_perigee_momentum (double);
    
  void EMconvertedphoton_perigee_Covd0d0 (double);    
  void EMconvertedphoton_perigee_Covd0z0 (double) ;    
  void EMconvertedphoton_perigee_Covd0phi (double) ;  
  void EMconvertedphoton_perigee_Covd0theta (double) ;
//   void EMconvertedphoton_perigee_Covd0Eclus (double) ;
  void EMconvertedphoton_perigee_Covz0z0   (double) ;
  void EMconvertedphoton_perigee_Covz0phi  (double) ;
  void EMconvertedphoton_perigee_Covz0theta (double) ;
//   void EMconvertedphoton_perigee_Covz0Eclus (double) ;
  void EMconvertedphoton_perigee_Covphiphi (double) ;
  void EMconvertedphoton_perigee_Covphitheta (double) ;
//   void EMconvertedphoton_perigee_CovphiEclus (double) ;
  void EMconvertedphoton_perigee_Covthetatheta (double) ;
//   void EMconvertedphoton_perigee_CovthetaEclus (double) ;
//   void EMconvertedphoton_perigee_CovEclusEclus (double) ;



  /***************************************************/
  /* Track related parameters  :   Get Parameters*/
  /***************************************************/

  /** @brief transverse impact parameter (distance of closest approach)  */
  double EMtrack_perigee_d0    ()  const;
  /** @brief azimuth angle of the momentum at the point of closest approach */
  double EMtrack_perigee_phi0  ()  const;
//   /** @brief cluster energy */
//   double EMtrack_perigee_Eclus () const;
  /** @brief the z value at the point of closest approach */
  double EMtrack_perigee_z0    ()  const;
  /** @brief theta of the track fit */
  double EMtrack_perigee_theta ()  const;
   /** @brief eta of the track fit */
  double EMtrack_perigee_eta    ()  const;
  /** @brief momentum of the track fit */
  double EMtrack_perigee_momentum ()  const; 
  
  /** @brief Covariance matrix item  (d0,d0)*/
  double EMtrack_perigee_Covd0d0         () const;
  /** @brief Covariance matrix item (d0,z0) */
  double EMtrack_perigee_Covd0z0         () const;
  /** @brief Covariance matrix item (d0,phi) */
  double EMtrack_perigee_Covd0phi        () const;
  /** @brief Covariance matrix item (d0,theta) */
  double EMtrack_perigee_Covd0theta      () const;
//   /** @brief Covariance matrix item (d0,Eclus) */
//   double EMtrack_perigee_Covd0Eclus     () const;
  /** @brief Covariance matrix item (z0,z0) */
  double EMtrack_perigee_Covz0z0         () const;
  /** @brief Covariance matrix item (z0,phi) */
  double EMtrack_perigee_Covz0phi        () const;
  /** @brief Covariance matrix item (z0,theta) */
  double EMtrack_perigee_Covz0theta      () const;
//   /** @brief Covariance matrix item (z0,Eclus) */
//   double EMtrack_perigee_Covz0Eclus     () const;
  /** @brief Covariance matrix item (phi,phi) */
  double EMtrack_perigee_Covphiphi       () const;
  /** @brief Covariance matrix item (phi,theta) */
  double EMtrack_perigee_Covphitheta     () const;
//   /** @brief Covariance matrix item (phi,Eclus) */
//   double EMtrack_perigee_CovphiEclus    () const;
  /** @brief Covariance matrix item (theta,theta) */
  double EMtrack_perigee_Covthetatheta   () const;
//   /** @brief Covariance matrix item (theta,Eclus) */
//   double EMtrack_perigee_CovthetaEclus  () const;
//   /** @brief Covariance matrix item (Eclus, Eclus) */
//   double EMtrack_perigee_CovEclusEclus () const;

      
  /***************************************************/
  /* Track related parameters  :   Set Parameters*/
  /***************************************************/


  /** @brief Set the transverse impact parameter (distance of closest approach)  */
  void EMtrack_perigee_d0    (double)  ;
  /** @brief Set the azimuth angle of the momentum at the point of closest approach */
  void EMtrack_perigee_phi0  (double)  ;
//   /** @brief Set the Eclus estimate according to track cluster */
//   void EMtrack_perigee_Eclus(double)  ;
  /** @brief Set the the z value at the point of closest approach*/
  void EMtrack_perigee_z0    (double)  ;
  /** @brief Set the theta of the track fit */
  void EMtrack_perigee_theta (double)  ;
   /** @brief Set the theta of the track fit */
   void EMtrack_perigee_eta    (double)  ;
  /** @brief Set the theta of the track fit */
  void EMtrack_perigee_momentum (double)  ;

  
  /** @brief Set the Covariance matrix item (d0,d0) */
  void EMtrack_perigee_Covd0d0         (double) ;
  /** @brief Set the Covariance matrix item (d0,z0) */
  void EMtrack_perigee_Covd0z0         (double) ;
  /** @brief Set the Covariance matrix item (d0,phi) */
  void EMtrack_perigee_Covd0phi        (double) ;
  /** @brief Set the Covariance matrix item (d0,theta) */
  void EMtrack_perigee_Covd0theta      (double) ;
//   /** @brief Set the Covariance matrix item (d0,Eclus) */
//   void EMtrack_perigee_Covd0Eclus     (double) ;
  /** @brief Set the Covariance matrix item (z0,z0) */
  void EMtrack_perigee_Covz0z0         (double) ;
  /** @brief Set the Covariance matrix item (z0,phi) */
  void EMtrack_perigee_Covz0phi        (double) ;
  /** @brief Set the Covariance matrix item (z0,theta) */
  void EMtrack_perigee_Covz0theta      (double) ;
//   /** @brief Set the Covariance matrix item (z0,Eclus) */
//   void EMtrack_perigee_Covz0Eclus     (double) ;
  /** @brief Set the Covariance matrix item (phi,phi) */
  void EMtrack_perigee_Covphiphi       (double) ;
  /** @brief Set the Covariance matrix item (phi,theta) */
  void EMtrack_perigee_Covphitheta     (double) ;
//   /** @brief Set the Covariance matrix item (phi,Eclus) */
//   void EMtrack_perigee_CovphiEclus    (double) ;
  /** @brief Set the Covariance matrix item (theta,theta) */
  void EMtrack_perigee_Covthetatheta   (double) ;
//   /** @brief Set the Covariance matrix item (theta,Eclus) */
//   void EMtrack_perigee_CovthetaEclus  (double) ;
//   /** @brief Set the Covariance matrix item (Eclus,Eclus) */
//   void EMtrack_perigee_CovEclusEclus (double) ; 

  /***************************************************/
  /* Photon related parameters : Get Parameters */
  /***************************************************/
    
  /** @brief  cluster eta */    
  double EMphoton_eta ()  const;
  /**  @brief  cluster phi */
  double EMphoton_phi0 ()  const;
  /** @brief  cluster energy */ 
  double EMphoton_Eclus ()  const;
   

  /** @brief Covariance matrix item (Eta,Eta) */
  double EMphoton_Covetaeta      () const;
  /** @brief Covariance matrix item (Eta,Phi) */
  double EMphoton_Covetaphi     () const;
  /** @brief Covariance matrix item (Eta,Eclus) */
  double EMphoton_CovetaEclus      () const;
  /** @brief Covariance matrix item (phi,phi) */
  double EMphoton_Covphiphi    () const;
  /** @brief Covariance matrix item (phi,Eclus) */
  double EMphoton_CovphiEclus    () const;
  /** @brief Covariance matrix item (theta,theta) */
  /** @brief Covariance matrix item (Eclus, Eclus) */
  double EMphoton_CovEclusEclus () const;


  /***************************************************/
  /* Photon related parameters : Set Parameters */
  /***************************************************/

  /** @brief  Set cluster eta */    
  void EMphoton_eta (double);
  /**  @brief  Set cluster phi */
  void EMphoton_phi0 (double);
  /** @brief  Set cluster energy */ 
  void EMphoton_Eclus (double);
   

  /** @brief Set covariance matrix item (Eta,Eta) */
  void EMphoton_Covetaeta      (double);
  /** @brief Set covariance matrix item (Eta,Phi) */
  void EMphoton_Covetaphi     (double);
  /** @brief Set covariance matrix item (Eta,Eclus) */
  void EMphoton_CovetaEclus      (double);
  /** @brief Set covariance matrix item (phi,phi) */
  void EMphoton_Covphiphi    (double);
  /** @brief Set covariance matrix item (phi,Eclus) */
  void EMphoton_CovphiEclus    (double);
  /** @brief Set covariance matrix item (Eclus, Eclus) */
  void EMphoton_CovEclusEclus (double);

  /***********************************************************************/
  /* Combined track & cluster related parameters : Set Parameters */
  /***********************************************************************/

  double EMtrack_comb_Covd0d0         () const;
  double EMtrack_comb_Covd0z0         () const;
  double EMtrack_comb_Covd0phi        () const;
  double EMtrack_comb_Covd0eta      () const;
  double EMtrack_comb_Covd0P     () const;
  double EMtrack_comb_Covz0z0         () const;
  double EMtrack_comb_Covz0phi        () const;
  double EMtrack_comb_Covz0eta      () const;
  double EMtrack_comb_Covz0P     () const;
  double EMtrack_comb_Covphiphi       () const;
  double EMtrack_comb_Covphieta     () const;
  double EMtrack_comb_CovphiP    () const;
  double EMtrack_comb_Covetaeta   () const;
  double EMtrack_comb_CovetaP  () const;
  double EMtrack_comb_CovPP () const;

  /** @brief Set the Combined Covariance matrix items */
  void EMtrack_comb_Covd0d0         (double) ;
  void EMtrack_comb_Covd0z0         (double) ;
  void EMtrack_comb_Covd0phi        (double) ;
  void EMtrack_comb_Covd0eta      (double) ;
  void EMtrack_comb_Covd0P     (double) ;
  void EMtrack_comb_Covz0z0         (double) ;
  void EMtrack_comb_Covz0phi        (double) ;
  void EMtrack_comb_Covz0eta      (double) ;
  void EMtrack_comb_Covz0P     (double) ;
  void EMtrack_comb_Covphiphi       (double) ;
  void EMtrack_comb_Covphieta     (double) ;
  void EMtrack_comb_CovphiP    (double) ;
  void EMtrack_comb_Covetaeta   (double) ;
  void EMtrack_comb_CovetaP  (double) ;
  void EMtrack_comb_CovPP (double) ; 


  /***************************************************/
  /* Link Index for tracks and vertices */
  /***************************************************/
 
  /** @brief  link index */    
  int linkIndex ()  const;
 
  /** @brief  Set link index */    
  void set_linkIndex (int);

  /** @brief  Does the track of vertex have silicon hits */
  int hasSiliconHits() const;
  /** @brief  set whether the track of vertex have silicon hits */
  void set_hasSiliconHits (int x);

  /** @brief get the position eta error; since this is used by the 
      EMExtrapolCaloConversion tool, it has to be public */
  double getClusterEtaPosError(const egamma*, 
			       const EMClusterErrorsParametrizations*,
			       bool forcePhoton = false) const;

private:

  void set_parameterInt(egammaParameters::ParamDef, int, bool overwrite=false) ; 

  /** @brief Get the error on cluster energy, eta and phi */
  double caloEta(const egamma*, double clusterEta) const;
  double getClusterEnergyError(const egamma*, const EMClusterErrorsParametrizations*, bool forcePhoton = false) const;
  double getClusterEtaError(const egamma*, const EMClusterErrorsParametrizations*, bool forcePhoton = false) const;
  double getClusterPhiError(const egamma*, const EMClusterErrorsParametrizations*, bool forcePhoton = false) const;

    
  /** Obsolete Fill the perigree parameter for converted photon */
  //template <int DIM, class T, class S> bool fillConvertedPhotonMatrixElements(const Trk::ParametersT< DIM, T, S>* ); 

  /** Obsolete Fill the perigree parameter for converted photon  */
  //bool fillConvertedPhotonErrorMatrix(const AmgSymMatrix(5)*);

  /** Obsolete Fill the perigree parameter details */
  //template <int DIM, class T, class S> bool fillTrackPerigeeMatrixElements(const Trk::ParametersT< DIM, T, S>* ) ;

  /** Obsolete Fill the perigree error details */
  //bool fillTrackPerigeeErrorMatrix(const AmgSymMatrix(5)*);

  /* Obsolete: Fill the photon parameter details */
  //bool fillPhotonMatrixElements(const egamma*, const EMClusterErrorsParametrizations*, 
  //				const Amg::Vector3D&, bool forcePhoton=false);
  //Obsolete Fill the perigree error details */
  //bool fillPhotonErrorMatrix(const egamma*, const EMClusterErrorsParametrizations*, bool);

  //** @brief is it an electron? */
  // Should maybe make it return electron, photon, or conversion in the future **/
  bool isElectron(const egamma*, bool forcePhoton) const;

  // jacobians
  AmgSymMatrix(5) P5Jacobiand0z0PhiThetaE2d0z0PhiEtaE(const double phi) const;
  AmgSymMatrix(4) P4JacobiandEThetaPhiM2EEtaPhiM(const double theta) const;

  std::vector< std::pair<egammaParameters::ParamDef,double> > m_parameters;
  std::vector< std::pair<egammaParameters::ParamDef,int> > m_parametersInt;
  static const std::string s_className;

};

#endif 


