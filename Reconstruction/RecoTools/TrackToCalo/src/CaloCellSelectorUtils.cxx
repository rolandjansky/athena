/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include <algorithm>
#include "TrackToCalo/CaloCellSelectorUtils.h"


namespace Utils
{
  double deltaPhi(double phi1, double phi2) {
    double dPhi=std::fabs(phi1-phi2);
    if (dPhi>M_PI) dPhi=2*M_PI-dPhi;
    return dPhi;
  }

  double deltaR2(double eta1, double eta2, double phi1, double phi2) {
    double dPhi=deltaPhi(phi1,phi2);
    double dEta=std::fabs(eta1-eta2);
    return dEta*dEta + dPhi*dPhi;
  }

  double deltaR(double eta1, double eta2, double phi1, double phi2) {
    return std::sqrt(deltaR2(eta1, eta2, phi1, phi2));
  }

  void findNearestPoint( const Amg::Vector3D& inputPos, const Trk::CaloExtension* caloExtension,
                         int&           nearestIdx,
                         Amg::Vector3D& nearestPos,
                         Amg::Vector3D& nearestMom ){

    const std::vector<Trk::CurvilinearParameters>& intersections = caloExtension->caloLayerIntersections();
    int nPts = intersections.size();

    int idxL, idxR, idxMid;
    Amg::Vector3D pos (0,0,0), mom (0,0,0); //initialization should be(?) unnecessary, just to suppress scary warning msg...
    Amg::Vector3D posL(0,0,0), momL(0,0,0);
    Amg::Vector3D posR(0,0,0), momR(0,0,0);

    // find nearest crossing point
    idxL = 0;
    idxR = nPts-1;
    while ((idxR-idxL)>1){
      idxMid = (idxL+idxR)/2;
      pos = intersections[idxMid].position();
      mom = intersections[idxMid].momentum();

      if ( (inputPos-pos).dot(mom)>0) { 
        idxL = idxMid;
        std::swap(posL, pos);
        std::swap(momL, mom);
      }
      else { 
        idxR = idxMid;
        std::swap(posR, pos);
        std::swap(momR, mom);
      }
    }

    if (idxL==0){
      posL = intersections[0].position();
      momL = intersections[0].momentum();
    }
    if (idxR==(nPts-1)){
      posR = intersections[nPts-1].position();
      momR = intersections[nPts-1].momentum();
    }

    float mag2L = (inputPos-posL).mag2();
    float mag2R = (inputPos-posR).mag2();
    nearestIdx = (mag2L<mag2R)? idxL : idxR;
    nearestPos = (mag2L<mag2R)? posL : posR;
    nearestMom = (mag2L<mag2R)? momL : momR;
  }
}
