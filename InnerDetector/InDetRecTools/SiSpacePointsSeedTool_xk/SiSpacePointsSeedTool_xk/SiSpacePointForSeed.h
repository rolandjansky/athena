/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiSpacePointForSeed
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class space points for seed maker 
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 18/11/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointForSeed_h
#define SiSpacePointForSeed_h
#include "TrkSpacePoint/SpacePoint.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetPrepRawData/SiCluster.h"
#include "TrkSurfaces/Surface.h"

namespace InDet {

  class SiSpacePointForSeed {
    
    /////////////////////////////////////////////////////////////////////////////////
    // Public methods:
    /////////////////////////////////////////////////////////////////////////////////
    
  public:
    
    SiSpacePointForSeed();
    SiSpacePointForSeed(Trk::SpacePoint*const&,const float*);
    SiSpacePointForSeed(Trk::SpacePoint*const&,const float*,const float*);
    SiSpacePointForSeed(const SiSpacePointForSeed&);
    virtual ~SiSpacePointForSeed()                 ;
    SiSpacePointForSeed& operator  = (const SiSpacePointForSeed&);

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

  protected:
    
    float m_x   ; // x-coordinate in beam system coordinates  
    float m_y   ; // y-coordinate in beam system coordinates
    float m_z   ; // z-coordinate in beam system coordinetes
    float m_r   ; // radius       in beam system coordinates
    float m_covr; //
    float m_covz; //
    float m_param;
    float m_q   ;
    const Trk::Surface* m_su;
    const Trk::Surface* m_sn;
  };


  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline SiSpacePointForSeed::SiSpacePointForSeed ()
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
   }

  inline SiSpacePointForSeed& SiSpacePointForSeed::operator = 
    (const SiSpacePointForSeed& sp) 
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
      }
      return(*this);
    }
 
  inline SiSpacePointForSeed::SiSpacePointForSeed
    (Trk::SpacePoint*const& sp,const float* r) 
    {
      set(sp,r); m_param = 0.;
    }

  inline SiSpacePointForSeed::SiSpacePointForSeed
    (Trk::SpacePoint*const& sp,const float* r,const float* sc) 
    {
      set(sp,r,sc); m_param = 0.;
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Copy constructor
  /////////////////////////////////////////////////////////////////////////////////

  inline SiSpacePointForSeed::SiSpacePointForSeed (const SiSpacePointForSeed& sp)
    {
      *this = sp;
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Destructor
  /////////////////////////////////////////////////////////////////////////////////

  inline SiSpacePointForSeed::~SiSpacePointForSeed() 
  {
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Set
  /////////////////////////////////////////////////////////////////////////////////

  inline void SiSpacePointForSeed::set
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

  inline void SiSpacePointForSeed::set
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
      }
      m_su = &sp->clusterList().first->detectorElement()->surface();
    }

  inline void SiSpacePointForSeed::setParam(const float& p)
    {
      m_param = p;
    }
  inline void  SiSpacePointForSeed::setQuality(float q)
    {
      if(q <= m_q) m_q = q;
    }
 
} // end of name space

#endif  // SiSpacePointForSeed_h
