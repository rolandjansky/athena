/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiSpacePointForSeedITK
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class space points for seed maker 
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 18/11/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointForSeedITK_h
#define SiSpacePointForSeedITK_h
#include "TrkSpacePoint/SpacePoint.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetPrepRawData/SiCluster.h"
#include "TrkSurfaces/Surface.h"

namespace InDet {

  class SiSpacePointForSeedITK {
    
    /////////////////////////////////////////////////////////////////////////////////
    // Public methods:
    /////////////////////////////////////////////////////////////////////////////////
    
  public:
    
    SiSpacePointForSeedITK();
    SiSpacePointForSeedITK(Trk::SpacePoint*const&,const float*);
    SiSpacePointForSeedITK(Trk::SpacePoint*const&,const float*,const float*);
    SiSpacePointForSeedITK(const SiSpacePointForSeedITK&);
    virtual ~SiSpacePointForSeedITK()                 ;
    SiSpacePointForSeedITK& operator  = (const SiSpacePointForSeedITK&);

    void set(Trk::SpacePoint*const&,const float*)  ;
    void set(Trk::SpacePoint*const&,const float*,const float*);
    void setQuality(float);
    void setParam(const float&);

    const Trk::SpacePoint* spacepoint              ; 
    const float&          x() const {return m_x;}
    const float&          y() const {return m_y;}
    const float&          z() const {return m_z;}
    const float&     radius() const {return m_r;}
          float         phi() const {return atan2(m_y,m_x);}
    const float&       covr() const {return m_covr;}
    const float&       covz() const {return m_covz;}
    const float&      param() const {return m_param;}
    const float&    quality() const {return m_q ;}
    const Trk::Surface* sur() const {return m_su;}
    const Trk::Surface* sun() const {return m_sn;}

    bool coordinates(float*,float*);

  protected:
    
    float m_x   ; // x-coordinate in beam system coordinates  
    float m_y   ; // y-coordinate in beam system coordinates
    float m_z   ; // z-coordinate in beam system coordinetes
    float m_r   ; // radius       in beam system coordinates
    float m_covr; //
    float m_covz; //
    float m_param;
    float m_q   ;

    float m_b0[3];
    float m_b1[3];
    float m_dr[3];
    float m_r0[3];

    const Trk::Surface* m_su;
    const Trk::Surface* m_sn;
  };


  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline SiSpacePointForSeedITK::SiSpacePointForSeedITK ()
    {
      spacepoint = 0;
      m_x     = 0.;
      m_y     = 0.;
      m_z     = 0.;
      m_r     = 0.;
      m_covr  = 0.;
      m_covz  = 0.;
      m_param = 0.;
      m_q     = 0.;
      m_su    = 0 ;
      m_sn    = 0 ;
      for(int i=0; i!=3; ++i) {m_b0[i]=0.; m_b1[i]=0.; m_dr[i]=0.; m_r0[i]=0.;}
   }

  inline SiSpacePointForSeedITK& SiSpacePointForSeedITK::operator = 
    (const SiSpacePointForSeedITK& sp) 
    {
      if(&sp!=this) {
	spacepoint  = sp.spacepoint;
	m_x         = sp.m_x       ;
	m_y         = sp.m_y       ;
	m_z         = sp.m_z       ;
	m_r         = sp.m_r       ;
	m_covr      = sp.m_covr    ;
	m_covz      = sp.m_covz    ;
	m_q         = sp.m_q       ;
	m_su        = sp.m_su      ;
	m_sn        = sp.m_sn      ;        
	for(int i=0; i!=3; ++i) m_b0[i]=sp.m_b0[i];
	for(int i=0; i!=3; ++i) m_b1[i]=sp.m_b1[i];
	for(int i=0; i!=3; ++i) m_dr[i]=sp.m_dr[i];
 	for(int i=0; i!=3; ++i) m_r0[i]=sp.m_r0[i];
       }
      return(*this);
    }
 
  inline SiSpacePointForSeedITK::SiSpacePointForSeedITK
    (Trk::SpacePoint*const& sp,const float* r) 
    {
      set(sp,r); m_param = 0.;
    }

  inline SiSpacePointForSeedITK::SiSpacePointForSeedITK
    (Trk::SpacePoint*const& sp,const float* r,const float* sc) 
    {
      set(sp,r,sc); m_param = 0.;
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Copy constructor
  /////////////////////////////////////////////////////////////////////////////////

  inline SiSpacePointForSeedITK::SiSpacePointForSeedITK (const SiSpacePointForSeedITK& sp)
    {
      *this = sp;
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Destructor
  /////////////////////////////////////////////////////////////////////////////////

  inline SiSpacePointForSeedITK::~SiSpacePointForSeedITK() 
  {
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Set
  /////////////////////////////////////////////////////////////////////////////////

  inline void SiSpacePointForSeedITK::set
    (Trk::SpacePoint*const& sp,const float* r)
    {
      spacepoint = sp  ;
      m_x        = r[0];
      m_y        = r[1];
      m_z        = r[2];
      m_r        =sqrt(m_x*m_x+m_y*m_y);
      m_q        = 100000.;

      const InDet::SiCluster*           c  = static_cast<const InDet::SiCluster*>(sp->clusterList().first);
      const InDetDD::SiDetectorElement* de = c ->detectorElement();

      if( de->isPixel() ) {
      
	const Amg::MatrixX& v =  c->localCovariance();
	float f22 = float(v(1,1) );
	float wid = float(c->width().z());
	float cov = wid*wid*.08333; if(cov < f22) cov = f22;
	if(de->isBarrel()) {m_covz = 9.*cov; m_covr = .06;}
	else               {m_covr = 9.*cov; m_covz = .06;}
	m_sn = 0;
      }
      else                {

	const Amg::MatrixX& v = sp->localCovariance();
	float f22 = float(v(1,1));
	if(de->isBarrel()) {m_covz = 8.*f22; m_covr = .1;} 
	else               {m_covr = 8.*f22; m_covz = .1;} 
	m_sn =  &sp->clusterList().second->detectorElement()->surface();

	for(int i=0; i!=3; ++i) {m_b0[i]=r[3 +i]; m_b1[i]=r[6 +i]; m_dr[i]=r[9 +i]; m_r0[i]=r[12+i];}
	
      }
      m_su = &sp->clusterList().first->detectorElement()->surface();
    } 

  /////////////////////////////////////////////////////////////////////////////////
  // Set with error correction 
  // sc[0] - barrel pixels error correction
  // sc[1] - endcap pixels 
  // sc[2] - barrel sct
  // sc[3] - endcap sct 
  /////////////////////////////////////////////////////////////////////////////////

  inline void SiSpacePointForSeedITK::set
    (Trk::SpacePoint*const& sp,const float* r,const float* sc)
    {
      spacepoint = sp  ;
      m_x        = r[0];
      m_y        = r[1];
      m_z        = r[2];
      m_r        =sqrt(m_x*m_x+m_y*m_y);
      m_q        = 100000.;

      const InDet::SiCluster*           c  = static_cast<const InDet::SiCluster*>(sp->clusterList().first);
      const InDetDD::SiDetectorElement* de = c ->detectorElement();

      if( de->isPixel() ) {
      
	const Amg::MatrixX& v =  c->localCovariance();
	float f22 = float(v(1,1));
	float wid = float(c->width().z());
	float cov = wid*wid*.08333; if(cov < f22) cov = f22;
	if(de->isBarrel()) {m_covz = 9.*cov*sc[0]; m_covr = .06;}
	else               {m_covr = 9.*cov*sc[1]; m_covz = .06;}
	m_sn = 0;
      }
      else                {

	const Amg::MatrixX& v = sp->localCovariance();
	float f22 = float(v(1,1));
	if(de->isBarrel()) {m_covz = 8.*f22*sc[2]; m_covr = .1;} 
	else               {m_covr = 8.*f22*sc[3]; m_covz = .1;} 
	m_sn =  &sp->clusterList().second->detectorElement()->surface();

	for(int i=0; i!=3; ++i) {m_b0[i]=r[3 +i]; m_b1[i]=r[6 +i]; m_dr[i]=r[9 +i]; m_r0[i]=r[12+i];}
      }
      m_su = &sp->clusterList().first->detectorElement()->surface();
    }

  inline void SiSpacePointForSeedITK::setParam(const float& p)
    {
      m_param = p;
    }
  inline void  SiSpacePointForSeedITK::setQuality(float q)
    {
      if(q <= m_q) m_q = q;
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Coordinate of cross points two SCT strip calculation for given direction
  // d - input direction
  // r - output coordinates
  // true if cross point is inside detector elements 
  /////////////////////////////////////////////////////////////////////////////////

  inline bool SiSpacePointForSeedITK::coordinates(float* d,float* r)
    {
      float d0[3] = {m_b1[1]*d[2]-m_b1[2]*d[1],m_b1[2]*d[0]-m_b1[0]*d[2],m_b1[0]*d[1]-m_b1[1]*d[0]};
      float bd0   =  m_b0[0]*d0[0]+m_b0[1]*d0[1]+m_b0[2]*d0[2];       if(     bd0==0.          ) return false;
      float s0    =-(m_dr[0]*d0[0]+m_dr[1]*d0[1]+m_dr[2]*d0[2])/bd0;  if(s0 < -.05 || s0 > 1.05) return false;

      float d1[3] = {m_b0[1]*d[2]-m_b0[2]*d[1],m_b0[2]*d[0]-m_b0[0]*d[2],m_b0[0]*d[1]-m_b0[1]*d[0]};
      float bd1   =  m_b1[0]*d1[0]+m_b1[1]*d1[1]+m_b1[2]*d1[2];       if(       bd1==0.        ) return false;
      float s1    = (m_dr[0]*d1[0]+m_dr[1]*d1[1]+m_dr[2]*d1[2])/bd1;  if(s1 < -.05 || s1 > 1.05) return false;
      
      r[0] = m_r0[0]+m_b0[0]*s0;
      r[1] = m_r0[1]+m_b0[1]*s0;
      r[2] = m_r0[2]+m_b0[2]*s0;
      return true;
    }

} // end of name space

#endif  // SiSpacePointForSeedITK_h
