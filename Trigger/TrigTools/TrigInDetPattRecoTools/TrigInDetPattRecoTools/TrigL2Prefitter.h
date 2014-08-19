/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_PREFITTER_H__
#define __TRIG_L2_PREFITTER_H__

class TrigL2TrackCandidate;

#include <cmath>
#include <vector>


class TrigL2Prefitter {
  
 public:
  
  TrigL2Prefitter(bool fieldOn=true,bool removeOutliers=true) :
    m_fieldOn(fieldOn), m_removeOutliers(removeOutliers)
    {};
    
    virtual ~TrigL2Prefitter(){};
    
    void run(TrigL2TrackCandidate* pTrack);

 private:
    
    bool m_fieldOn, m_removeOutliers;
  
};

#endif
