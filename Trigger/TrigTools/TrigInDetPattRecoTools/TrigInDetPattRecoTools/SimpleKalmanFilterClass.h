/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __SIMPLE_KALMAN_FILTER_CLASS_H__
#define __SIMPLE_KALMAN_FILTER_CLASS_H__

class TrigL2TrackCandidate;
class TrigSiSpacePointBase;

//#include <cmath>
//#include <vector>


class SimpleKalmanFilterClass {
  
 public:
  
  SimpleKalmanFilterClass(double c) : m_fConst(c){};
    ~SimpleKalmanFilterClass(){};
    
    void newTrack(class TrigL2TrackCandidate*);
    double getChi2(const TrigSiSpacePointBase*);
    void update();
    
    double m_Rk[5],m_Gk[5][5],m_Hk[2][5],m_Kk[5][2],m_resid[2],
      m_Bk[5][2],m_Vk[2][2],m_sigma[2][2],m_Dk[2][2], 
      m_fConst, m_dchi2, m_detr, m_x, m_y, m_averagePhi0;
};


#endif
