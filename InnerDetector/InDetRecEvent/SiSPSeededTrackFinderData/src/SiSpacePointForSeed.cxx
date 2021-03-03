/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>



#include "SiSPSeededTrackFinderData/SiSpacePointForSeed.h"

#include "InDetPrepRawData/SiCluster.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "TrkSurfaces/Surface.h"

namespace InDet {

  SiSpacePointForSeed::SiSpacePointForSeed ()
  {
    spacepoint = nullptr;
    m_x     = 0.;
    m_y     = 0.;
    m_z     = 0.;
    m_r     = 0.;
    m_covr  = 0.;
    m_covz  = 0.;
    m_param = 0.;
    m_d0    = 0.;
    m_eta   = 0.;
    m_dzdr  = 0.;
    m_pt    = 0.;
    m_q     = 0.;
    m_su    = nullptr ;
    m_sn    = nullptr ;
  }

  SiSpacePointForSeed& SiSpacePointForSeed::operator = 
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
      m_dzdr      = sp.m_dzdr    ;
      m_d0        = sp.m_d0      ;
      m_eta       = sp.m_eta     ;
      m_pt        = sp.m_pt      ;
      m_su        = sp.m_su      ;
      m_sn        = sp.m_sn      ;        
    }
    return(*this);
  }
 
  SiSpacePointForSeed::SiSpacePointForSeed
  (const Trk::SpacePoint*const& sp,const float* r) 
  {
    set(sp,r); m_param = 0.;  
  }

  SiSpacePointForSeed::SiSpacePointForSeed
  (const Trk::SpacePoint*const& sp,const float* r,const float* sc) 
  {
    set(sp,r,sc); m_param = 0.; 
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Copy constructor
  /////////////////////////////////////////////////////////////////////////////////

  SiSpacePointForSeed::SiSpacePointForSeed (const SiSpacePointForSeed& sp)
  {
    *this = sp;
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Destructor
  /////////////////////////////////////////////////////////////////////////////////

  SiSpacePointForSeed::~SiSpacePointForSeed() 
  {
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Set
  /////////////////////////////////////////////////////////////////////////////////

  void SiSpacePointForSeed::set
  (const Trk::SpacePoint*const& sp,const float* r)
  {
    spacepoint = sp  ;
    m_x        = r[0];
    m_y        = r[1];
    m_z        = r[2];
    m_r        =std::sqrt(m_x*m_x+m_y*m_y);
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
      m_sn = nullptr;
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

  void SiSpacePointForSeed::set
  (const Trk::SpacePoint*const& sp,const float* r,const float* sc)
  {
    spacepoint = sp  ;
    m_x        = r[0];
    m_y        = r[1];
    m_z        = r[2];
    m_r        =std::sqrt(m_x*m_x+m_y*m_y);
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
      m_sn = nullptr;
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

  void SiSpacePointForSeed::setParam(const float& p)
  {
    m_param = p;
  }

  void SiSpacePointForSeed::setD0(const float& d0)
  {
    m_d0 = d0;
  } 

  void SiSpacePointForSeed::setEta(const float& eta)
  {
    m_eta = eta;
  }

  void  SiSpacePointForSeed::setQuality(float q)
  {
    if(q <= m_q) m_q = q;
  }

  void  SiSpacePointForSeed::setDZDR(const float& dzdr)
  {
    m_dzdr = dzdr;
  }

  void  SiSpacePointForSeed::setPt(const float& pt)
  {
    m_pt = pt;
  }
 
} // end of name space
