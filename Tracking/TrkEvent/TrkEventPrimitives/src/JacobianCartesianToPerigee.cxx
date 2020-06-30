/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianCartesianToPerigee.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkEventPrimitives/JacobianCartesianToPerigee.h"
#include <cmath>

Trk::JacobianCartesianToPerigee::JacobianCartesianToPerigee(const double px, const double py, const double pz, const double E, const double, const double, const double, const double charge ):
  AmgMatrix(5,7)()
{
  // initialize to zero  
  this->setZero();
      
  double ptot = std::sqrt(px*px+py*py+pz*pz);
  double pt =   std::sqrt(px*px+py*py);

  double tan_theta = pt/pz;
  
  double cos_phi = px/pt;
  double sin_phi = py/pt;

  (*this)(0,4) =  -sin_phi ; // dd0/dx
  (*this)(0,5) =  cos_phi ; // dd0/dy 
  (*this)(1,6) =  1.; // dz0/dz
  (*this)(2,0) =  -py/(px*px+py*py) ; // dphi/dpx 
  (*this)(2,1) =  px/(px*px+py*py) ; // dphi/dpy 
  (*this)(3,0) =  (cos_phi/pz)*(1./(1+tan_theta*tan_theta)); // dtheta/dpx
  (*this)(3,1) =  (sin_phi/pz)*(1./(1+tan_theta*tan_theta)); // dtheta/dpy
  (*this)(3,2) =  (-tan_theta/pz)*(1./(1+tan_theta*tan_theta)); // dtheta/dpz
  (*this)(4,3) =  -charge*E/(ptot*ptot*ptot); // dqOverP/dE

}

