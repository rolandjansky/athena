/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTPCNV_MERGEDEVENTINFO_P2_H
#define EVENTTPCNV_MERGEDEVENTINFO_P2_H

/**
 * @file MergedEventInfo_p2.h
 *
 * @brief This class is the persistent representation of MergedEventInfo
 *
 * @author R.D.Schaffer@cern.ch
 */

#include "EventTPCnv/EventInfo_p3.h"
// #include "EventTPCnv/EventID_p2.h"

class MergedEventInfo_p2 : public EventInfo_p3 {
public:
  MergedEventInfo_p2() {};  ///< POOL required
  virtual ~MergedEventInfo_p2() {};

  std::vector<unsigned int> m_newEventIDdata;
};

#endif 


