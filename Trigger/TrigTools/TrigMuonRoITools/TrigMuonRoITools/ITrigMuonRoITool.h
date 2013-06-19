/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIGMUONROITOOL_H__ 
#define __ITRIGMUONROITOOL_H__ 

#include "GaudiKernel/IAlgTool.h"
#include <vector>
#include <utility>

namespace ROIB {
  class MuCTPIRoI;
}

class ITrigMuonRoITool: virtual public IAlgTool 
{
 public:
  /**
   * @brief ITrigMuonRoITool Interface ID 
   */
  static const InterfaceID& interfaceID() { 
    static InterfaceID s_iTrigMuonRoIToolID("ITrigMuonRoITool", 1 , 0); 
    return s_iTrigMuonRoIToolID; 
  }

  /**
   * @brief Accessor to an iterator over all RoIs in time with the event
   * @return handle to the begin iterator
   */
  virtual std::vector<ROIB::MuCTPIRoI>::const_iterator begin_InTimeRoIs() = 0;

  /**
   * @brief Accessor to an iterator over all RoIs in time with the event
   * @return handle to the end iterator
   */
  virtual std::vector<ROIB::MuCTPIRoI>::const_iterator end_InTimeRoIs() = 0;

  /**
   * @brief Accessor to an iterator over all RoIs out of time with the event.
   * @brief pair < Muon RoI in RoIB format, difference: RoI(BCID)-event(BCID) > 
   * @return handle to the begin iterator
   */
  virtual std::vector< std::pair<ROIB::MuCTPIRoI,int> >::const_iterator begin_OutOfTimeRoIs() = 0;

  /**
   * @brief Accessor to an iterator over all RoIs out of time with the event.
   * @brief pair < Muon RoI in RoIB format, difference: RoI(BCID)-event(BCID) > 
   * @return handle to the end iterator
   */
  virtual std::vector< std::pair<ROIB::MuCTPIRoI,int> >::const_iterator end_OutOfTimeRoIs() = 0;
};
#endif
