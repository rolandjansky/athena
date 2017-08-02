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
    
    SiSpacePointForSeedITK* m_s0;
    SiSpacePointForSeedITK* m_s1;
    SiSpacePointForSeedITK* m_s2;
    float                   m_z ;
    float                   m_q ;
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
