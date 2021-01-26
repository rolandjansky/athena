// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

namespace InDet {
  class SiSpacePointForSeed;
  class SiSpacePointsSeed;

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
    const float&            d0() const {return m_d0;}
    const float&             z() const {return m_z ;}
    const float&           eta() const {return m_eta;}
    const float&            z1() const {return m_z1;}
    const float&            z2() const {return m_z2;}
    const float&            z3() const {return m_z3;}
    const float&            r1() const {return m_r1;}
    const float&            r2() const {return m_r2;}
    const float&            r3() const {return m_r3;}
    const float&       quality() const {return m_q ;}
    
    void set(SiSpacePointForSeed*&,SiSpacePointForSeed*&,SiSpacePointForSeed*&,float);
    bool setQuality(float);
    bool set3(InDet::SiSpacePointsSeed&);
    void set2(InDet::SiSpacePointsSeed&);

  private:
    
    SiSpacePointForSeed* m_s0  ;
    SiSpacePointForSeed* m_s1  ;
    SiSpacePointForSeed* m_s2  ;
    float                m_d0  ;
    float                m_z   ;
    float                m_eta ;
    float                m_z1  ;
    float                m_z2  ;
    float                m_z3  ;
    float                m_r1  ;
    float                m_r2  ;
    float                m_r3  ;
    float                m_q   ;
  };
} // end of name space

#endif  // SiSpacePointsProSeed_h
