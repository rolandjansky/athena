// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_EMCONVERT_H
#define EGAMMAEVENT_EMCONVERT_H 1
/**
  @class EMConvert
          EM convert property class data class.
   @author 
*/
/********************************************************************
NAME:     EMConvert.h
PACKAGE:  offline/Reconstruction/egammaEvent
//   EM convert property class data class. 
//    it is empty until the converted tracks are available.
//  
//   Updated September 22, 2005 by Dave Joffe  
          Jun 14, 2007 (FD) Doxygen documentation
          Jul 30, 2010 (JM) Add ambiguity result
********************************************************************/

#include "egammaEvent/egDetail.h"
#include "TrkParameters/TrackParameters.h"

#include <vector>

namespace Trk{
  class VxCandidate;
}

class EMConvert : public egDetail
{
 public:

  /** @brief Default constructor*/
  EMConvert();

  /** @brief Destructor*/
  virtual ~EMConvert();
  virtual const std::string& className() const;
  
  /** @brief general parameter retrieval*/
  virtual double parameter(egammaParameters::ParamDef) const ;
  /** @brief general parameter setting */
  void set_parameter(egammaParameters::ParamDef, double, bool overwrite=false) ;

  virtual bool boolParameter(egammaParameters::ParamDef) const ;
  virtual int intParameter(egammaParameters::ParamDef) const ;
  virtual bool hasParameter(egammaParameters::ParamDef) const ;
  virtual bool hasBoolParameter(egammaParameters::ParamDef) const ;
  virtual bool hasIntParameter(egammaParameters::ParamDef) const ;
  
  void print() const; 

/*   /\** @brief fill the details from a Trk::VxCandidate *\/ */
  void fillVxDetails(const Trk::VxCandidate *vxCand, int trkAuthor);

  /** @brief get the VxCandidate from the stored egammaParameters */
  Trk::VxCandidate* getVxCandidate() const;

  // return values of conversion parameters
  /** @brief conversion track matches egamma track*/
  bool convTrackMatch() const;   
  /** @brief conversion angle matches egamma angle*/
  bool convAngleMatch() const;   

  /** @brief Delta eta of match between track or photon and cluster*/
  double convMatchDeltaEta1() const;   
  /** @brief Delta phi of match between track or photon and cluster*/
  double convMatchDeltaPhi1() const;   
  /** @brief Delta eta of match between 2nd track and cluster*/
  double convMatchDeltaEta2() const;   
  /** @brief Delta phi of match between 2nd track and cluster*/
  double convMatchDeltaPhi2() const;   

  /** @brief Delta eta of match between track or photon and cluster (after refit)*/
  double convMatchDeltaEta1_refit() const;   
  /** @brief Delta phi of match between track or photon and cluster (after refit)*/
  double convMatchDeltaPhi1_refit() const;   
  /** @brief Delta eta of match between 2nd track and cluster (after refit)*/
  double convMatchDeltaEta2_refit() const;   
  /** @brief Delta phi of match between 2nd track and cluster (after refit)*/
  double convMatchDeltaPhi2_refit() const;   

  /** @brief vertex position (cartesian coordinates) */
  double vertex_x() const;
  double vertex_y() const;
  double vertex_z() const;
  /** @brief vertex position covariance matrix (cartesian coordinates) */
  double vertex_Covxx() const;  
  double vertex_Covxy() const;  
  double vertex_Covxz() const;  
  double vertex_Covyy() const;  
  double vertex_Covyz() const;  
  double vertex_Covzz() const;  
  /** @brief vertex fit quality */
  double vertex_chi2() const;  
  int vertex_ndf() const;  
  int vertex_numTracks() const;  

  /** @brief track1 global momentum (cartesian coordinates) */
  double vertex_track1_phi() const;
  double vertex_track1_theta() const;
  double vertex_track1_qOverP() const;
  /** @brief track1 chi2 */
  double vertex_track1_chi2() const;
  /** @brief track1 author */
  int vertex_track1_author() const;

  /** @brief track2 global momentum (cartesian coordinates) */
  double vertex_track2_phi() const;
  double vertex_track2_theta() const;
  double vertex_track2_qOverP() const;
  /** @brief track2 chi2 */
  double vertex_track2_chi2() const;
  /** @brief track2 author */
  int vertex_track2_author() const;

  /** @brief refitted track covariance matrix and correlations with vertex position */
  double vertex_Covd01d01() const;
  double vertex_Covd01z01() const;
  double vertex_Covd01phi1() const;
  double vertex_Covd01theta1() const;
  double vertex_Covd01qOverP1() const;
  double vertex_Covz01z01() const;
  double vertex_Covz01phi1() const;
  double vertex_Covz01theta1() const;
  double vertex_Covz01qOverP1() const;
  double vertex_Covphi1phi1() const;
  double vertex_Covphi1theta1() const;
  double vertex_Covphi1qOverP1() const;
  double vertex_Covtheta1theta1() const;
  double vertex_Covtheta1qOverP1() const;
  double vertex_CovqOverP1qOverP1() const;
  double vertex_Covphi1phi2() const;
  double vertex_Covphi1theta2() const;
  double vertex_Covphi1qOverP2() const;
  double vertex_Covtheta1phi2() const;
  double vertex_Covtheta1theta2() const;
  double vertex_Covtheta1qOverP2() const;
  double vertex_CovqOverP1phi2() const;
  double vertex_CovqOverP1theta2() const;
  double vertex_CovqOverP1qOverP2() const;
  double vertex_Covphi1x() const;
  double vertex_Covphi1y() const;
  double vertex_Covphi1z() const;
  double vertex_Covtheta1x() const;
  double vertex_Covtheta1y() const;
  double vertex_Covtheta1z() const;
  double vertex_CovqOverP1x() const;
  double vertex_CovqOverP1y() const;
  double vertex_CovqOverP1z() const;
  double vertex_Covd02d02() const;
  double vertex_Covd02z02() const;
  double vertex_Covd02phi2() const;
  double vertex_Covd02theta2() const;
  double vertex_Covd02qOverP2() const;
  double vertex_Covz02z02() const;
  double vertex_Covz02phi2() const;
  double vertex_Covz02theta2() const;
  double vertex_Covz02qOverP2() const;
  double vertex_Covphi2phi2() const;
  double vertex_Covphi2theta2() const;
  double vertex_Covphi2qOverP2() const;
  double vertex_Covtheta2theta2() const;
  double vertex_Covtheta2qOverP2() const;
  double vertex_CovqOverP2qOverP2() const;
  double vertex_Covphi2x() const;
  double vertex_Covphi2y() const;
  double vertex_Covphi2z() const;
  double vertex_Covtheta2x() const;
  double vertex_Covtheta2y() const;
  double vertex_Covtheta2z() const;
  double vertex_CovqOverP2x() const;
  double vertex_CovqOverP2y() const;
  double vertex_CovqOverP2z() const;
  int ambiguityResult() const;
  int linkIndex() const;

  void set_convTrackMatch(bool);
  void set_convAngleMatch(bool);
  void set_convMatchDeltaEta1(double);
  void set_convMatchDeltaPhi1(double);
  void set_convMatchDeltaEta2(double);
  void set_convMatchDeltaPhi2(double);
  void set_convMatchDeltaEta1_refit(double);
  void set_convMatchDeltaPhi1_refit(double);
  void set_convMatchDeltaEta2_refit(double);
  void set_convMatchDeltaPhi2_refit(double);
  void set_vertex_x(double);
  void set_vertex_y(double);
  void set_vertex_z(double);
  void set_vertex_Covxx(double);
  void set_vertex_Covxy(double);
  void set_vertex_Covxz(double);
  void set_vertex_Covyy(double);
  void set_vertex_Covyz(double);
  void set_vertex_Covzz(double);
  void set_vertex_chi2(double);  
  void set_vertex_ndf(int);  
  void set_vertex_numTracks(int);  
  void set_vertex_track1_phi(double);
  void set_vertex_track1_theta(double);
  void set_vertex_track1_qOverP(double);
  void set_vertex_track1_chi2(double);
  void set_vertex_track1_author(int);
  void set_vertex_track2_phi(double);
  void set_vertex_track2_theta(double);
  void set_vertex_track2_qOverP(double);
  void set_vertex_track2_chi2(double);
  void set_vertex_track2_author(int);
  void set_vertex_Covd01d01(double);
  void set_vertex_Covd01z01(double);
  void set_vertex_Covd01phi1(double);
  void set_vertex_Covd01theta1(double);
  void set_vertex_Covd01qOverP1(double);
  void set_vertex_Covz01z01(double);
  void set_vertex_Covz01phi1(double);
  void set_vertex_Covz01theta1(double);
  void set_vertex_Covz01qOverP1(double);
  void set_vertex_Covphi1phi1(double);
  void set_vertex_Covphi1theta1(double);
  void set_vertex_Covphi1qOverP1(double);
  void set_vertex_Covtheta1theta1(double);
  void set_vertex_Covtheta1qOverP1(double);
  void set_vertex_CovqOverP1qOverP1(double);
  void set_vertex_Covphi1phi2(double);
  void set_vertex_Covphi1theta2(double);
  void set_vertex_Covphi1qOverP2(double);
  void set_vertex_Covtheta1phi2(double);
  void set_vertex_Covtheta1theta2(double);
  void set_vertex_Covtheta1qOverP2(double);
  void set_vertex_CovqOverP1phi2(double);
  void set_vertex_CovqOverP1theta2(double);
  void set_vertex_CovqOverP1qOverP2(double);
  void set_vertex_Covphi1x(double);
  void set_vertex_Covphi1y(double);
  void set_vertex_Covphi1z(double);
  void set_vertex_Covtheta1x(double);
  void set_vertex_Covtheta1y(double);
  void set_vertex_Covtheta1z(double);
  void set_vertex_CovqOverP1x(double);
  void set_vertex_CovqOverP1y(double);
  void set_vertex_CovqOverP1z(double);
  void set_vertex_Covd02d02(double);
  void set_vertex_Covd02z02(double);
  void set_vertex_Covd02phi2(double);
  void set_vertex_Covd02theta2(double);
  void set_vertex_Covd02qOverP2(double);
  void set_vertex_Covz02z02(double);
  void set_vertex_Covz02phi2(double);
  void set_vertex_Covz02theta2(double);
  void set_vertex_Covz02qOverP2(double);
  void set_vertex_Covphi2phi2(double);
  void set_vertex_Covphi2theta2(double);
  void set_vertex_Covphi2qOverP2(double);
  void set_vertex_Covtheta2theta2(double);
  void set_vertex_Covtheta2qOverP2(double);
  void set_vertex_CovqOverP2qOverP2(double);
  void set_vertex_Covphi2x(double);
  void set_vertex_Covphi2y(double);
  void set_vertex_Covphi2z(double);
  void set_vertex_Covtheta2x(double);
  void set_vertex_Covtheta2y(double);
  void set_vertex_Covtheta2z(double);
  void set_vertex_CovqOverP2x(double);
  void set_vertex_CovqOverP2y(double);
  void set_vertex_CovqOverP2z(double);
  void set_ambiguityResult(int);
  void set_linkIndex(int);

 private: 


  /** @brief fill the details with dummy values (so we can avoid uninitialized values when the conversion refit is not executed for some conversions) *\/ */
  void fillDummy();
  /** @brief fill the vertex variables from a Trk::VxCandidate *\/ */
  void fillVertex(const Trk::VxCandidate *vxCand);
  void fillVxTracks(const Trk::VxCandidate *vxCand, int trkAuthor);

  void set_parameterBool(egammaParameters::ParamDef, bool, bool overwrite=false) ; 
  void set_parameterInt(egammaParameters::ParamDef, int, bool overwrite=false) ; 
  std::vector< std::pair<egammaParameters::ParamDef,bool> > m_parametersBool;
  std::vector< std::pair<egammaParameters::ParamDef,int> > m_parametersInt; 
  std::vector< std::pair<egammaParameters::ParamDef,double> > m_parameters;
  
  /** @brief get the vertex position error matrix from the stored egammaParameters */
  AmgSymMatrix(3)* getVxPosErrorMatrix() const;
  /** @brief get the error matrix for the first track from the stored egammaParameters */
  AmgSymMatrix(5)  getTrk1ErrorMatrix() const;
  /** @brief get the error matrix for the second track from the stored egammaParameters */
  AmgSymMatrix(5) getTrk2ErrorMatrix() const;
  /** @brief get the full vertex error matrix from the stored egammaParameters */
  Amg::MatrixX* getVxErrorMatrix() const;
  
};

#endif 


