/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIGSISPACEPOINTBASE_H__
#define __TRIGSISPACEPOINTBASE_H__

#include <cmath>
#include "TrkSpacePoint/SpacePoint.h"

#define MAX_SILICON_LAYER_NUM   19

// layer numbering scheme:
//   3 Barrel Pixels 0,1,2           
//   4 Barrel SCT    3,4,5,6                     i.e. offsetBarrelSCT    =  3
//   3 Endcap Pixels 7,8,9                       i.e. offsetEndcapPixels =  7
//   9 Endcap SCT    10,11,12,13,14,15,16,17,18  i.e. offsetEndcapSCT    = 10

#define OffsetEndcapPixels  7
#define OffsetBarrelSCT     3
#define OffsetEndcapSCT    10

class TrigSiSpacePointBase {

public:

  // using cylindrical co-ordinates, no. errors
  TrigSiSpacePointBase(long layer,
		       double r,  double phi,  double z,
		       double dr=0.0, double dz=0.0, const Trk::SpacePoint* offlineSpacePoint = nullptr) :
    m_layer(layer),
    m_r(r), m_phi(phi), m_z(z),
    m_dr(dr), m_dz(dz), 
    m_offlineSpacePoint(offlineSpacePoint)
    {
      m_x = r * std::cos(phi);
      m_y = r * std::sin(phi);
      m_barCode=-1;
      if (m_offlineSpacePoint) {
        m_isPixel = (m_offlineSpacePoint->clusterList().second==nullptr);
      }
      else {
        m_isPixel = false;//Arbitrarily choose value when no offline spacepoint
      }
    }

  // Destructor
    virtual ~TrigSiSpacePointBase() {};
  
  // Methods to set data members
  void r(  const double r  ) {m_r   = r;  }
  void phi(const double phi) {m_phi = phi;}
  void z(  const double z  ) {m_z   = z;  }
  void x(  const double x  ) {m_x   = x;  }
  void y(  const double y  ) {m_y   = y;  }
  void dr(  const double dr  ) {m_dr   = dr;  }
  void dz(  const double dz  ) {m_dz   = dz;  }

  void barCode(int code) {m_barCode = code;}

  double r()    const {return m_r;}
  double phi()  const {return m_phi;}
  double z()    const {return m_z;}
  double dr()   const {return m_dr;} 
  double dz()   const {return m_dz;}
  double x()    const {return m_x;}
  double y()    const {return m_y;}
  long layer()  const {return m_layer;}

  bool isPixel() const {return  m_isPixel;}
  bool isSCT()   const {return !m_isPixel;}

  // Methods to calculate associated values

  double eta(double z0) const {
    double zr = (m_z-z0)/m_r; 
    return log(zr+std::sqrt(1.+zr*zr));
  }

  int barCode() const {return m_barCode;}
  const Trk::SpacePoint* offlineSpacePoint() const {return m_offlineSpacePoint;}

protected:

  long m_layer;

  double	m_r;
  double	m_x;
  double	m_y;
  
  double	m_phi;
  double	m_z;
  double	m_dr;
  double	m_dz;

  int           m_barCode;   //MC truth association
  bool          m_isPixel; //Stores whether spacepoint is Pixel or SCT

	const Trk::SpacePoint* m_offlineSpacePoint;
};
#endif 




