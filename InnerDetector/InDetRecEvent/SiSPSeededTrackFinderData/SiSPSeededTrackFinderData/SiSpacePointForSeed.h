// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include <cmath>

namespace Trk {
  class SpacePoint;
  class Surface;
}

namespace InDet {
  class SiCluster;

  class SiSpacePointForSeed {
    
    /////////////////////////////////////////////////////////////////////////////////
    // Public methods:
    /////////////////////////////////////////////////////////////////////////////////
    
  public:
    
    SiSpacePointForSeed();
    SiSpacePointForSeed(const Trk::SpacePoint*const&,const float*);
    SiSpacePointForSeed(const Trk::SpacePoint*const&,const float*,const float*);
    SiSpacePointForSeed(const SiSpacePointForSeed&);
    virtual ~SiSpacePointForSeed();
    SiSpacePointForSeed& operator  = (const SiSpacePointForSeed&);

    void set(const Trk::SpacePoint*const&,const float*);
    void set(const Trk::SpacePoint*const&,const float*,const float*);
    void setQuality(float);
    void setParam(const float&);

    const Trk::SpacePoint* spacepoint; 
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

  private:
    
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
 
} // end of name space

#endif  // SiSpacePointForSeed_h
