/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIG_L2_SEEDED_PATT_RECO_STRATEGY_H__
#define __ITRIG_L2_SEEDED_PATT_RECO_STRATEGY_H__

#include<vector>
#include<algorithm>
#include<iterator>

#include "GaudiKernel/IAlgTool.h"
#include "TrigSteeringEvent/Enums.h"

class TrigSiSpacePoint;
class IRoiDescriptor;
class TrigInDetTrackCollection;

static const InterfaceID IID_ITrigL2SeededPattRecoStrategy("ITrigL2SeededPattRecoStrategy",1,0);  
  
class ITrigL2SeededPattRecoStrategy: virtual public IAlgTool { 
 public: 
  static const InterfaceID& interfaceID() { 
    return IID_ITrigL2SeededPattRecoStrategy; 
  } 
  virtual HLT::ErrorCode findTracks(const TrigInDetTrackCollection&, 
				    const std::vector<const TrigSiSpacePoint*>&, 
				    const IRoiDescriptor&,
				    TrigInDetTrackCollection*) = 0;

  virtual HLT::ErrorCode findTracks(const TrigInDetTrackCollection&,
				    const std::vector<const TrigSiSpacePoint*>&,
				    TrigInDetTrackCollection*) = 0;

  virtual double getTimingA() const  = 0;
  void getZvertices(std::vector<float>& v) {
    v.clear();
    v.reserve(m_zVertices.size());
    std::copy(m_zVertices.begin(),m_zVertices.end(),std::back_inserter(v));
  }

 protected:

  std::vector<float> m_zVertices;// vector of z-vertex coordinates for monitoring
};

#endif


 
