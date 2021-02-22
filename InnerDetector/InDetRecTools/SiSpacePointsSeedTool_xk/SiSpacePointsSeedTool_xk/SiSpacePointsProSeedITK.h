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

#ifndef SiSpacePointsProSeedITK_h
#define SiSpacePointsProSeedITK_h

#include "SiSpacePointsSeedTool_xk/SiSpacePointForSeedITK.h"
#include "SiSpacePointsSeed/SiSpacePointsSeed.h"

namespace InDet {

  class SiSpacePointsProSeedITK {
    
    /////////////////////////////////////////////////////////////////////////////////
    // Public methods:
    /////////////////////////////////////////////////////////////////////////////////
    
  public:
    
    SiSpacePointsProSeedITK();

    SiSpacePointsProSeedITK(SiSpacePointForSeedITK*&,SiSpacePointForSeedITK*&,SiSpacePointForSeedITK*&,float);
    SiSpacePointsProSeedITK(const SiSpacePointsProSeedITK&);
    virtual ~SiSpacePointsProSeedITK();
    SiSpacePointsProSeedITK& operator  = (const SiSpacePointsProSeedITK&);

    SiSpacePointForSeedITK* spacepoint0() {return m_s0;}
    SiSpacePointForSeedITK* spacepoint1() {return m_s1;}
    SiSpacePointForSeedITK* spacepoint2() {return m_s2;}
    const float&             z() const {return m_z ;}
    const float&       quality() const {return m_q ;}
    
    void set(SiSpacePointForSeedITK*&,SiSpacePointForSeedITK*&,SiSpacePointForSeedITK*&,float);
    bool setQuality(float);
    bool set3(InDet::SiSpacePointsSeed&);
    void set2(InDet::SiSpacePointsSeed&);

  protected:
    
    SiSpacePointForSeedITK* m_s0    ;
    SiSpacePointForSeedITK* m_s1    ;
    SiSpacePointForSeedITK* m_s2    ;
    float                   m_z     ;
    float                   m_q     ;
  };


  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline SiSpacePointsProSeedITK::SiSpacePointsProSeedITK ()
    {
      m_s0 = 0 ;
      m_s1 = 0 ;
      m_s2 = 0 ;
      m_z  = 0.;
      m_q  = 0.;
    }

  inline SiSpacePointsProSeedITK& SiSpacePointsProSeedITK::operator = 
    (const SiSpacePointsProSeedITK& sp) 
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
 
  inline SiSpacePointsProSeedITK::SiSpacePointsProSeedITK
    (SiSpacePointForSeedITK*& s0,SiSpacePointForSeedITK*& s1,SiSpacePointForSeedITK*& s2,float z)
    {
      set(s0,s1,s2,z); m_q = 0.;
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Copy constructor
  /////////////////////////////////////////////////////////////////////////////////

  inline SiSpacePointsProSeedITK::SiSpacePointsProSeedITK (const SiSpacePointsProSeedITK& sp): m_s0(sp.m_s0),m_s1(sp.m_s1),m_s2(sp.m_s2)
    {
      *this = sp;
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Destructor
  /////////////////////////////////////////////////////////////////////////////////

  inline SiSpacePointsProSeedITK::~SiSpacePointsProSeedITK() 
  {
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Set 
  /////////////////////////////////////////////////////////////////////////////////

  inline void SiSpacePointsProSeedITK::set
    (SiSpacePointForSeedITK*& s0,SiSpacePointForSeedITK*& s1,SiSpacePointForSeedITK*& s2,float z)
    {
      m_z   = z ;
      m_s0  = s0;
      m_s1  = s1;
      m_s2  = s2;
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Set two space points seed
  /////////////////////////////////////////////////////////////////////////////////

  inline void SiSpacePointsProSeedITK::set2(InDet::SiSpacePointsSeed& s)
    {
      s.erase();
      s.add(m_s0->spacepoint);
      s.add(m_s1->spacepoint);
      s.setZVertex(double(m_z));
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Set three space points seed
  /////////////////////////////////////////////////////////////////////////////////

  inline bool SiSpacePointsProSeedITK::set3(InDet::SiSpacePointsSeed& s)
    {
            
      if(!m_s2->spacepoint->clusterList().second) {
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
      s.setD0(m_s2->param());
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
      s.setPt(m_s2->pt()); 
      return true;
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Set quality in pro seed
  /////////////////////////////////////////////////////////////////////////////////

  inline bool SiSpacePointsProSeedITK::setQuality(float q)
    {
      m_q = q;
      
      if(!m_s2->spacepoint->clusterList().second) {
	    if(q > m_s0->quality() && q > m_s1->quality() && q > m_s2->quality()) return false;
      }
      m_s0->setQuality(m_q);
      m_s1->setQuality(m_q);
      m_s2->setQuality(m_q);
      return true;
    }

} // end of name space

#endif  // SiSpacePointsProSeedITK_h
