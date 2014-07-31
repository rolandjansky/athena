/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTPCNV_MERGEDEVENTINFO_P1_H
#define EVENTTPCNV_MERGEDEVENTINFO_P1_H

/**
 * @file MergedEventInfo_p1.h
 *
 * @brief This class is the persistent representation of MergedEventInfo
 *
 * @author Marcin.Nowak@cern.ch
 */

#include "EventTPCnv/EventInfo_p1.h"
#include "EventTPCnv/EventID_p1.h"

class MergedEventInfo_p1 : public EventInfo_p1 {
public:
  MergedEventInfo_p1() {};  ///< POOL required
  virtual ~MergedEventInfo_p1() {};

  EventID_p1 	m_newEventID;
};

#endif 


