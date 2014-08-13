/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  SiSpacePointsComparison_h
#define  SiSpacePointsComparison_h

#include "TrkSpacePoint/SpacePoint.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointForSeed.h"

namespace InDet{


  ///////////////////////////////////////////////////////////////////
  // Object function for ordering space point in X coordinate order
  ///////////////////////////////////////////////////////////////////
  
  class SiSpacePointsComparison_X {
    
  public:
    bool operator () (const Trk::SpacePoint* p1,const Trk::SpacePoint* p2) {
      return((*p1).globalPosition().x()<(*p2).globalPosition().x());
    }
  };
  
  ///////////////////////////////////////////////////////////////////
  // Object function for ordering space point in Y coordinate order
  ///////////////////////////////////////////////////////////////////
  
  class SiSpacePointsComparison_Y {
    
  public:
    bool operator () (const Trk::SpacePoint* p1,const Trk::SpacePoint* p2) {
      return((*p1).globalPosition().y()<(*p2).globalPosition().y());
    }
  };
  
  ///////////////////////////////////////////////////////////////////
  // Object function for ordering space point in Z coordinate order
  ///////////////////////////////////////////////////////////////////
  
  class SiSpacePointsComparison_Z {
    
  public:
    bool operator () (const Trk::SpacePoint* p1,const Trk::SpacePoint* p2) {
      return((*p1).globalPosition().z()<(*p2).globalPosition().z());
    }
  };
  
  ///////////////////////////////////////////////////////////////////
  // Object function for ordering space point in azimuthal angle  order
  ///////////////////////////////////////////////////////////////////
  
  class SiSpacePointsComparison_Phi {
    
  public:
    bool operator () (const Trk::SpacePoint* p1,const Trk::SpacePoint* p2) {
      return((*p1).globalPosition().phi()<(*p2).globalPosition().phi());
    }
  };
  
  ///////////////////////////////////////////////////////////////////
  // Object function for ordering space point in radius  order
  ///////////////////////////////////////////////////////////////////
  
  class SiSpacePointsComparison_R {
  
  public:
    bool operator () (const Trk::SpacePoint* p1,const Trk::SpacePoint* p2) {
      return((*p1).globalPosition().perp()<(*p2).globalPosition().perp());
    }
  };

  ///////////////////////////////////////////////////////////////////
  // Object function for ordering space point seed in Z  order
  ///////////////////////////////////////////////////////////////////
  
  class SiSpacePointsSeedComparison_Z {
  
  public:

    bool operator () (InDet::SiSpacePointsSeed* s1,InDet::SiSpacePointsSeed* s2) {

      return((*s1).zVertex() < (*s2).zVertex());
    }
  };
}
#endif // SiSpacePointsComparison_h
