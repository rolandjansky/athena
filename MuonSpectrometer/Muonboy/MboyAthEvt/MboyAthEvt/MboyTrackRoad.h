/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MBOYTRACKROAD_H
#define MBOYTRACKROAD_H

#include "TrkRoad/TrackRoad.h"

  /**
   @class MboyTrackRoad

  @author samusog@cern.ch
  
  */

class MboyTrackRoad: public Trk::TrackRoad {
public:
   MboyTrackRoad();
   MboyTrackRoad(const Amg::Vector3D& gpos , const Amg::Vector3D& gdir, double dEta, double dPhi, double wei=1.0);
   virtual ~MboyTrackRoad();

public:
///////////////////////////////////
 
  double weight() const;
  std::string toString() const;
   
protected:
  double m_wei;
   
};
#endif
