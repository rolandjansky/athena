/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIGSISPACEPOINTBASE_H__
#define __TRIGSISPACEPOINTBASE_H__

#include <math.h>
#include <cstddef>
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
		       double dr=0.0, double dphi=0.0, double dz=0.0, const Trk::SpacePoint* offlineSpacePoint = NULL) :
    m_layer(layer),
    m_r(r), m_phi(phi), m_z(z),
    m_dr(dr), m_dphi(dphi), m_dz(dz), m_locT(0.), m_locL(0.), 
    m_original_r(r),  m_original_phi(phi), m_offlineSpacePoint(offlineSpacePoint)
    {
      m_x = m_original_x = r * cos(phi);
      m_y = m_original_y = r * sin(phi);
      m_elementIndex[0]=-1;m_elementIndex[1]=-1;
      m_eta = eta(0.0);
      m_rotatedPhi = m_phi;
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
  void dphi(const double dphi) {m_dphi = dphi;}
  void dz(  const double dz  ) {m_dz   = dz;  }
  void locT(const double locT) {m_locT = locT;}
  void locL(  const double locL  ) {m_locL   = locL;  }
  void elementIndex(int i, unsigned int idx)  {
    if((i==0) || (i==1)) 
      m_elementIndex[i]=idx;
  }

  void setEta(double eta) {
    m_eta=eta;
  }

  void usedBy(int idx) {m_usedBy=idx;}
  void index(int idx) {m_index=idx;}
  void rotatedPhi(double phi) {m_rotatedPhi = phi;}

  void barCode(int code) {m_barCode = code;}

  // Methods to return values of data members
  unsigned int elementIndex(int i) const {
    if((i==0) || (i==1)) 
      return m_elementIndex[i];
    else return -1;
  }

  double r()    const {return m_r;}
  double phi()  const {return m_phi;}
  double z()    const {return m_z;}
  double dr()   const {return m_dr;} 
  double dphi() const {return m_dphi;}
  double dz()   const {return m_dz;}
  double locT() const {return m_locT;}
  double locL() const {return m_locL;}
  double x()    const {return m_x;}
  double y()    const {return m_y;}
  long layer()  const {return m_layer;}
  double eta()  const {return m_eta;}

  double original_r() const {return  m_original_r;}
  double original_phi() const {return  m_original_phi;}
  double original_x() const {return  m_original_x;}
  double original_y() const {return  m_original_y;}

  bool isPixel() const {return  m_isPixel;}
  bool isSCT()   const {return !m_isPixel;}

  // Methods to calculate associated values

  double eta(double z0) const {
    double zr = (m_z-z0)/m_r; 
    return log(zr+sqrt(1.+zr*zr));
  }

  double rotatedPhi() const {return m_rotatedPhi;}
  int usedBy() const {return m_usedBy;}
  int index()  const {return m_index;}
  int barCode() const {return m_barCode;}
  const Trk::SpacePoint* offlineSpacePoint() const {return m_offlineSpacePoint;}

protected:

  unsigned int  m_elementIndex[2];
  long m_layer;

  double	m_r;
  double	m_x;
  double	m_y;
  
  double	m_phi;
  double	m_z;
  double	m_dr;
  double	m_dphi;
  double	m_dz;
  double	m_locT;
  double	m_locL;

  double        m_original_r; 
  double        m_original_phi; 
  double        m_original_x;
  double        m_original_y;

  double        m_rotatedPhi;// to be used for RoIs with [+/- pi] boundary
  double        m_eta;       //cache for eta result;
  int           m_usedBy;    //index of a track sp is assigned to
  int           m_index;     //sp index in a collection
  int           m_barCode;   //MC truth association
  bool          m_isPixel; //Stores whether spacepoint is Pixel or SCT

	const Trk::SpacePoint* m_offlineSpacePoint;
};
#endif 




