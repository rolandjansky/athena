/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiSpacesProSeed
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class space point seed for seed maker 
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 27/02/2013 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsProSeed_h
#define SiSpacePointsProSeed_h

#include "SiSpacePointsSeedTool_xk/SiSpacePointForSeed.h"
#include "SiSpacePointsSeed/SiSpacePointsSeed.h"

namespace InDet {

  class SiSpacePointsProSeed {
    
    /////////////////////////////////////////////////////////////////////////////////
    // Public methods:
    /////////////////////////////////////////////////////////////////////////////////
    
  public:
    
    SiSpacePointsProSeed();

    SiSpacePointsProSeed(SiSpacePointForSeed*&,SiSpacePointForSeed*&,SiSpacePointForSeed*&,float);
    SiSpacePointsProSeed(const SiSpacePointsProSeed&);
    virtual ~SiSpacePointsProSeed();
    SiSpacePointsProSeed& operator  = (const SiSpacePointsProSeed&);

    SiSpacePointForSeed* spacepoint0() {return m_s0;}
    SiSpacePointForSeed* spacepoint1() {return m_s1;}
    SiSpacePointForSeed* spacepoint2() {return m_s2;}
    const float&             z() const {return m_z ;}
    const float&       quality() const {return m_q ;}
    
    void set(SiSpacePointForSeed*&,SiSpacePointForSeed*&,SiSpacePointForSeed*&,float);
    bool setQuality(float);
    bool set3(InDet::SiSpacePointsSeed&);
    void set2(InDet::SiSpacePointsSeed&);

  protected:
    
    SiSpacePointForSeed* m_s0;
    SiSpacePointForSeed* m_s1;
    SiSpacePointForSeed* m_s2;
    float                m_z ;
    float                m_q ;
  };


  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline SiSpacePointsProSeed::SiSpacePointsProSeed ()
    {
      m_s0 = 0 ;
      m_s1 = 0 ;
      m_s2 = 0 ;
      m_z  = 0.;
      m_q  = 0.;
    }

  inline SiSpacePointsProSeed& SiSpacePointsProSeed::operator = 
    (const SiSpacePointsProSeed& sp) 
    {
      if(&sp!=this) {

	m_z   = sp.m_z ;
	m_q   = sp.m_q ;
	m_s0  = sp.m_s0;
	m_s1  = sp.m_s1;
	m_s2  = sp.m_s2;
      }
      return(*this);
    }
 
  inline SiSpacePointsProSeed::SiSpacePointsProSeed
    (SiSpacePointForSeed*& s0,SiSpacePointForSeed*& s1,SiSpacePointForSeed*& s2,float z)
    {
      set(s0,s1,s2,z); m_q = 0.;
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Copy constructor
  /////////////////////////////////////////////////////////////////////////////////

  inline SiSpacePointsProSeed::SiSpacePointsProSeed (const SiSpacePointsProSeed& sp): m_s0(sp.m_s0),m_s1(sp.m_s1),m_s2(sp.m_s2)
    {
      *this = sp;
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Destructor
  /////////////////////////////////////////////////////////////////////////////////

  inline SiSpacePointsProSeed::~SiSpacePointsProSeed() 
  {
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Set 
  /////////////////////////////////////////////////////////////////////////////////

  inline void SiSpacePointsProSeed::set
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

  inline void SiSpacePointsProSeed::set2(InDet::SiSpacePointsSeed& s)
    {
      s.erase();
      s.add(m_s0->spacepoint);
      s.add(m_s1->spacepoint);
      s.setZVertex(double(m_z));
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Set three space points seed
  /////////////////////////////////////////////////////////////////////////////////

  inline bool SiSpacePointsProSeed::set3(InDet::SiSpacePointsSeed& s)
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
      s.setZVertex(double(m_z)); 
      return true;
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Set quality in pro seed
  /////////////////////////////////////////////////////////////////////////////////

  inline bool SiSpacePointsProSeed::setQuality(float q)
    {
      m_q = q;
      bool pixb = !m_s0->spacepoint->clusterList().second;
      bool pixt = !m_s2->spacepoint->clusterList().second;
      if(pixb==pixt) {
	m_s0->setQuality(q);
	m_s1->setQuality(q);
	m_s2->setQuality(q);
	return true;
      }
      if(q < m_s0->quality() || q < m_s1->quality() || q < m_s2->quality()) return true;
      return false;
    }

} // end of name space

#endif  // SiSpacePointsProSeed_h
