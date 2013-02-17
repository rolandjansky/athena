/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __CombinedIDHalfTrackMuonObject_h__
#define __CombinedIDHalfTrackMuonObject_h__
/*
  CombinedIDHalfTrackMuonObject.h
*/
#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"
#include <string>
#include <vector>
#include <set>


class CombinedIDHalfTrackMuonObject {

public:
  /** c'tor */
   CombinedIDHalfTrackMuonObject(double av_phi, double av_Eta, double av_Theta,double id_Pt, double d_phi, double d_Eta, double d_Theta ) :
     d_phi0(d_phi), d_eta(d_Eta), d_theta(d_Theta), av_phi0(av_phi), av_eta(av_Eta),av_theta(av_Theta),id_pt(id_Pt){};


  ~CombinedIDHalfTrackMuonObject(){};

  /** function that returns the deltaPhi0 between the Muon and ID track */
  double delta_phi0() {return d_phi0;}

  /** function that returns the deltaEta between the Muon and ID track */
  double delta_eta() {return d_eta;}

  /** function that returns the deltaTheta between the Muon and ID track */
  double delta_theta() {return d_theta;}

  /** function that returns the average / combined Phi0 of the two combined tracks */
  double avtrk_phi0() {return av_phi0;}

  /** function that returns the average / combined Eta of the two combined tracks */
  double avtrk_eta() {return av_eta;}

  /** function that returns the average / combined Theta of the two combined tracks */
  double avtrk_theta() {return av_theta;}

  /** function that returns the average / combined Pt of the two combined tracks */
  double idpt() {return id_pt;};

 private:

  /** delta phi0 between muon and track*/
  double d_phi0;

  /** delta eta between muon and track*/
  double d_eta;

  /** delta Pt between muon and track*/
  double  d_pt;

  /** delta theta between muon and track*/
  double  d_theta;

  /** average / combined phi0 of the muon and track */
  double  av_phi0; 

  /** average / combined eta of the muon and track */
  double  av_eta; 

  /** average / combined theta of the muon and track */
  double  av_theta; 

  /** average / combined Pt of the muon and track */
  double  id_pt;

};

CLASS_DEF( CombinedIDHalfTrackMuonObject , 175628195, 1 ) 
CLASS_DEF( DataVector<CombinedIDHalfTrackMuonObject> ,  31229518, 1 )  

#endif // __CombinedIDMuonObject_hxx__
