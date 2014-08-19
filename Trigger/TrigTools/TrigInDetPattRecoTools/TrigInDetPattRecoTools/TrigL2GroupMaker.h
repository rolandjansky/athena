/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_GROUP_MAKER_H__
#define __TRIG_L2_GROUP_MAKER_H__

#include "TrigInDetPattRecoTools/TrigL2GroupMakerBase.h"

//this class is intended for Athena-based running

class TrigRoiDescriptor;

class TrigL2GroupMaker : public TrigL2GroupMakerBase {
 public:
  TrigL2GroupMaker(TrigL2HoughTransformClass* pHT, TrigL2HashKeyFunction* pHK, float maxEta, double maxShift, 
		   bool enhanceL0=false) : TrigL2GroupMakerBase(pHT, pHK, maxEta, maxShift, enhanceL0){
  }
    virtual ~TrigL2GroupMaker(){};

    using TrigL2GroupMakerBase::makeTrackCandidates;
    using TrigL2GroupMakerBase::minPhi;

    void makeTrackCandidates(std::vector<TrigSiSpacePointBase*>&, std::vector<TrigL2TrackCandidate*>&, 
			     double, //zVertex 
			     double, //layerThreshold_tmp
			     const IRoiDescriptor&
			     );			    
};

#endif
