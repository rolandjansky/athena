/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SiSPSeededTrackFinderData/SiSpacePointsProSeed.h"

#include "SiSpacePointsSeed/SiSpacePointsSeed.h"
#include "SiSPSeededTrackFinderData/SiSpacePointForSeed.h"

namespace InDet {
  SiSpacePointsProSeed::SiSpacePointsProSeed ()
  {
    m_s0 = nullptr ;
    m_s1 = nullptr ;
    m_s2 = nullptr ;
    m_d0 = 0.;
    m_eta = 0.;
    m_z  = 0.;
    m_q  = 0.;
  }

  SiSpacePointsProSeed& SiSpacePointsProSeed::operator = 
  (const SiSpacePointsProSeed& sp) 
  {
    if(&sp!=this) {

      m_d0  = sp.m_d0;
      m_z   = sp.m_z ;
      m_eta = sp.m_eta;
      m_q   = sp.m_q ;
      m_s0  = sp.m_s0;
      m_s1  = sp.m_s1;
      m_s2  = sp.m_s2;
    }
    return(*this);
  }
 
  SiSpacePointsProSeed::SiSpacePointsProSeed
  (SiSpacePointForSeed*& s0,SiSpacePointForSeed*& s1,SiSpacePointForSeed*& s2,float z)
  {
    set(s0,s1,s2,z); m_q = 0.;
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Copy constructor
  /////////////////////////////////////////////////////////////////////////////////

  SiSpacePointsProSeed::SiSpacePointsProSeed (const SiSpacePointsProSeed& sp): m_s0(sp.m_s0),m_s1(sp.m_s1),m_s2(sp.m_s2)
  {
    *this = sp;
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Destructor
  /////////////////////////////////////////////////////////////////////////////////

  SiSpacePointsProSeed::~SiSpacePointsProSeed() 
  {
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Set 
  /////////////////////////////////////////////////////////////////////////////////

  void SiSpacePointsProSeed::set
  (SiSpacePointForSeed*& s0,SiSpacePointForSeed*& s1,SiSpacePointForSeed*& s2,float z)
  {
    m_z   = z ;
    m_s0  = s0;
    m_s1  = s1;
    m_s2  = s2;
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Set two space points seed
  /////////////////////////////////////////////////////////////////////////////////

  void SiSpacePointsProSeed::set2(InDet::SiSpacePointsSeed& s)
  {
    s.erase();
    s.add(m_s0->spacepoint);
    s.add(m_s1->spacepoint);
    s.setZVertex(double(m_z));
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Set three space points seed
  /////////////////////////////////////////////////////////////////////////////////

  bool SiSpacePointsProSeed::set3(InDet::SiSpacePointsSeed& s)
  {
      
    bool pixb = !m_s0->spacepoint->clusterList().second;
    bool pixt = !m_s2->spacepoint->clusterList().second;
      
    if(pixb!=pixt) {
      if(m_q > m_s0->quality() && m_q > m_s1->quality() && m_q > m_s2->quality()) return false;
    }
     
    m_s0->setQuality(m_q);
    m_s1->setQuality(m_q);
    m_s2->setQuality(m_q);
      
    s.erase();
    s.add(m_s0->spacepoint);
    s.add(m_s1->spacepoint);
    s.add(m_s2->spacepoint);
    s.setD0(m_s2->param());
    s.setZVertex(double(m_z));
    s.setEta(m_s2->eta()); 
    s.setX1(m_s0->x());
    s.setX2(m_s1->x());
    s.setX3(m_s2->x());
    s.setY1(m_s0->y());
    s.setY2(m_s1->y());
    s.setY3(m_s2->y());
    s.setZ1(m_s0->z());
    s.setZ2(m_s1->z());
    s.setZ3(m_s2->z());
    s.setR1(m_s0->radius());
    s.setR2(m_s1->radius());
    s.setR3(m_s2->radius());
    s.setDZDR_B(m_s0->dzdr());
    s.setDZDR_T(m_s2->dzdr());
    s.setPt(m_s2->Pt()); 
    return true;
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Set quality in pro seed
  /////////////////////////////////////////////////////////////////////////////////

  bool SiSpacePointsProSeed::setQuality(float q)
  {
    m_q = q;
    bool pixb = !m_s0->spacepoint->clusterList().second;
    bool pixt = !m_s2->spacepoint->clusterList().second;
    /// if PPP or SSS, just update quality, don't cut
    if(pixb==pixt) {
      m_s0->setQuality(q);
      m_s1->setQuality(q);
      m_s2->setQuality(q);
      return true;
    }
    return q < m_s0->quality() || q < m_s1->quality() || q < m_s2->quality();
  }

} // end of name space
