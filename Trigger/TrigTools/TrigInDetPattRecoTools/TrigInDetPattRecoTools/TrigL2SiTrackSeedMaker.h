/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_SITRACK_SEED_MAKER_H__
#define __TRIG_L2_SITRACK_SEED_MAKER_H__

#include<cmath>
#include<exception>

class TrigL2SpacePointStorage;
class TrigL2SiTrackSeedingCuts;
class TrigL2TrackCandidate;
class TrigL2TimeoutException;
class TrigL2SiLayerRoad;

class TrigL2SiTrackSeedMaker {

 public:
  TrigL2SiTrackSeedMaker(const TrigL2SpacePointStorage& sps, 
			 const TrigL2SiTrackSeedingCuts& c, 
			 TrigL2TrackCandidate* p,
			 unsigned int& trackNum) : m_spLay(sps), m_cuts(c), m_trks(p), m_trks_num(trackNum) {
    m_trks_num=0;
  }

  ~TrigL2SiTrackSeedMaker(){};

  void operator()(const TrigL2SiLayerRoad&) throw(TrigL2TimeoutException);

 protected:

  const TrigL2SpacePointStorage& m_spLay;
  const TrigL2SiTrackSeedingCuts& m_cuts;
  TrigL2TrackCandidate*  m_trks;
  unsigned int& m_trks_num;
};

#endif
