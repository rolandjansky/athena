// -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

namespace InDet {
  class SiSpacePointForSeedITK;
  class SiSpacePointsSeed;

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

  private:
    
    SiSpacePointForSeedITK* m_s0{};
    SiSpacePointForSeedITK* m_s1{};
    SiSpacePointForSeedITK* m_s2{};
    float                   m_z{} ;
    float                   m_q{} ;
  };
} // end of name space

#endif  // SiSpacePointsProSeedITK_h
