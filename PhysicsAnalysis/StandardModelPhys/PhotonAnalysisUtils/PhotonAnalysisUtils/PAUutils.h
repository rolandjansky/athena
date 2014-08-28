/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHOTONANALYSISUTILS_PAUUTILS_H
#define PHOTONANALYSISUTILS_PAUUTILS_H

#include <math.h>

namespace PAUutils {

  inline float deltaR(float eta1, float phi1, float eta2, float phi2) {
    return sqrt( 2 * ( cosh(eta1-eta2) - cos(phi1-phi2) ) ) ;
  }

  template<class T1,class T2> inline float deltaR(const T1* p1, const T2* p2) {
    return deltaR(p1->eta(),p1->phi(),p2->eta(),p2->phi()) ;
  }

  template<class T1,class T2> inline int closestWRTdeltaR(const T1* pRef, const std::vector<const T2*>& pList, float& dRbest) {
    /** return index of closest; max allowed deltaR can be set through argument dRbest in input */
    int iClosest = -1 ;
    if ( dRbest<=0 )  dRbest = +9.999e+10 ;
    for ( unsigned u=0 ; u<pList.size() ; ++u ) {
      float dR = deltaR(pRef,pList[u]) ;
      if ( dR < dRbest ) {
	iClosest = u ;
	dRbest = dR ;
      }
    }
    if ( iClosest < 0 )  dRbest = +9.999e+10 ; // not found...
    return iClosest ;
  }

  /*=========================================================================================================================================*/

  static bool gimeRZ(int layer, float eta, float& R, float& Z) {
    const float etaBarrelEnd = 1.475 ;
    const float etaAB        = 0.8 ;
    const float eta2PS       = 1.8 ;
    const float eta2EM       = 2.5 ;
    R = Z = -9999. ;
    float aeta = fabs(eta) ;
    if ( aeta > eta2EM )  return false ;
    float sign = (eta>=0) ? 1 : -1 ;
    float Sinh = sinh(eta) ;
    switch ( layer ) {
    case 0 : // PS -----------------------------------------
      if ( aeta < etaBarrelEnd ) {
	R = 144.3 ;
	Z = R * Sinh ;
      } else if ( aeta < eta2PS ) {
	Z = 362.5 * sign ;
	R = Z / Sinh ;
      } else {
	return false ;
      }
      break ;
    case 1 : // Strips -------------------------------------
      if ( aeta < etaAB ) {
	R = 156.78 - 1.8975*aeta - 1.7668*pow(aeta,2) ;
	Z = R * Sinh ;
      } else if ( aeta < etaBarrelEnd ) {
	R = 150.32 + 7.1716*aeta - 4.1008*pow(aeta,2) ;
	Z = R * Sinh ;
      } else {
	Z = 376. * sign ;
	R = Z / Sinh ;
      }
      break ;
    case 2 : // Middle -------------------------------------
      if ( aeta < etaAB ) {
	R = 169.71 - 1.5311*aeta - 6.4153*pow(aeta,2) ;
	Z = R * Sinh ;
      } else if ( aeta < etaBarrelEnd ) {
	R = 173.91 - 7.5648*aeta - 1.8501*pow(aeta,2) ;
	Z = R * Sinh ;
      } else {
	Z = 388. * sign ;
	R = Z / Sinh ;
      }
      break ;
    default: 
      return false ;
    } ;
    R *= 10 ; // convert to mm
    Z *= 10 ; // convert to mm
    return true ;
  } 

  inline float EtaFromVertex(int layer, float eta, float Zvtx) {
    float R, Z ;
    if ( ! gimeRZ(layer,eta,R,Z) ) return -99999. ;
    return asinh( (Z-Zvtx) / R ) ;
  }


template <class T> class High2LowByPt {
 public:
  bool operator () (const T *t1, const T *t2) const {
    if ( t1 == t2 )  return false ;
    volatile double pt1 = t1->pt() ; // prevents optimization
    volatile double pt2 = t2->pt() ; // prevents optimization
    return ( pt1 > pt2 ) ;
  }
};



}

#endif // PHOTONANALYSISUTILS_PAUUTILS_H
