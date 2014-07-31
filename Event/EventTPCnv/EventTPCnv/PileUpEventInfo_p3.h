/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTPCNV_PILEUPEVENTINFO_P3_H
#define EVENTTPCNV_PILEUPEVENTINFO_P3_H
/**
 * @file PileUpEventInfo_p2.h
 *
 * @brief This class is the persistent representation of PileUpEventInfo_p2
 *
 * @author R.D.Schaffer@cern.ch
 */

#include "EventTPCnv/EventInfo_p2.h"
#include <vector>

class MsgStream;

class PileUpEventInfo_p3 : public EventInfo_p2 {
public:

  PileUpEventInfo_p3() {};
  virtual ~PileUpEventInfo_p3() {};
        
  struct SubEvent {
    short		m_time;
    short               m_index;
    short               m_type;
    EventInfo_p2	m_subEventInfo; 
  };

  typedef std::vector<SubEvent>	SubEvVect_t;
  SubEvVect_t		m_subEvents; 
};


#endif


