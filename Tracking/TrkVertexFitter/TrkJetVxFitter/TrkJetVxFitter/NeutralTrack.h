/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VXJETVERTEX_NEUTRACK_H
#define VXJETVERTEX_NEUTRACK_H
#include "EventPrimitives/EventPrimitives.h"

namespace Trk {
  class NeutralTrack : std::pair<Amg::Vector3D,Amg::Vector3D> {
    
  public:
    NeutralTrack(Amg::Vector3D x,Amg::Vector3D p);

    ~NeutralTrack() {};
    
    const Amg::Vector3D & position() const {
      return this->first;
    }
    
    const Amg::Vector3D & momentum() const {
      return this->second;
    }

  };

  inline NeutralTrack::NeutralTrack(Amg::Vector3D x,Amg::Vector3D p): 
    std::pair<Amg::Vector3D,Amg::Vector3D>(x,p) {
  }
}


#endif
