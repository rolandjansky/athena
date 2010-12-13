/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_EVENTMBTS_H
#define ANP_EVENTMBTS_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : EventMBTS
 * @Author : Josh Kunkle
 *
 * @Brief  : MBTS data for 32 pads for one event
 * 
 **********************************************************************************/

// C/C++
#include <vector>

namespace Anp
{
  class EventMBTS {
  public:
    
    EventMBTS();
    ~EventMBTS() {}
        
  private:

    std::vector<float>     fChargeVec;
    std::vector<float>     fTimeVec;

  private : 
    int GetIndex(int type, int chan, int module) const;
    int GetType(int index) const;
    int GetChannel(int index) const;
    int GetModule(int index) const;
    
  public :

    void SetCharge(int type, int chan, int mod, float charge);
    void SetTime(int type, int chan, int mod, float time);
    
    //    GetCharge(type) -> Get charge sum for side A or C
    //    GetCharge(type, channel) -> Get charge sum over 8 modules in type, channel
    //    GetCharge(type, channel, module)->Get charge for single module
    float GetCharge(int tp, int ch = -1, int mod = -1) const;
    float GetChargeCut(float cut, int tp, int ch = -1, int mod = -1) const;
    
    //    GetTime(type) -> Get average time for side A or C
    //    GetTime(type, channel) -> get average time over 8 modules in type, channel
    //    GetTime(type, channel, module)->Get time for single module
    float GetTime(int tp, int ch = -1, int mod = -1) const;
    
    // same scheme as above, but time is weighted by charge 
    float GetWeightedTime(int tp, int ch = -1, int mod = -1) const;    
  };
}

#endif
