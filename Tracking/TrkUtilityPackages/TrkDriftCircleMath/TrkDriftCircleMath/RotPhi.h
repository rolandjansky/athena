/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_ROTPHI_H
#define DCMATH_ROTPHI_H

#include <cmath>

#include "TrkDriftCircleMath/Rot2D.h"
#include "TrkDriftCircleMath/LocVec2D.h"

namespace TrkDriftCircleMath {
 
  class RotPhi {
  public:
    RotPhi( double phi )
      : m_phi(phi),m_cosphi( cos(phi) ),m_sinphi( sin(phi) ) {}
    
    double phi() const { return m_phi; }
    double cosphi() const { return m_cosphi; }
    double sinphi() const { return m_sinphi; }

    double xval( const LocVec2D& lv ) const { return  cosphi()*lv.x() + sinphi()*lv.y(); }
    double yval( const LocVec2D& lv ) const { return -sinphi()*lv.x() + cosphi()*lv.y(); }
    
    RotPhi invers() const { return RotPhi( -phi() ); }

    void set( double phi ){
      m_phi = phi;
      m_cosphi = cos(phi);
      m_sinphi = sin(phi);
    }
  private:
    double m_phi;
    double m_cosphi;
    double m_sinphi;
  };

}

TrkDriftCircleMath::LocVec2D operator*( const TrkDriftCircleMath::RotPhi& rot, 
					const TrkDriftCircleMath::LocVec2D& lv );


#endif
