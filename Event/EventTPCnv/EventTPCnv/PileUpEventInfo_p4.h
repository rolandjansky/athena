/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTPCNV_PILEUPEVENTINFO_P4_H
#define EVENTTPCNV_PILEUPEVENTINFO_P4_H
/**
 * @file PileUpEventInfo_p4.h
 *
 * @brief This class is the persistent representation of PileUpEventInfo_p4
 *
 * @author ilija@vukotic.me
 */

#include "EventTPCnv/EventInfo_p3.h"
#include <vector>

class MsgStream;

class PileUpEventInfo_p4 : public EventInfo_p3 {
public:

  PileUpEventInfo_p4() {};
  virtual ~PileUpEventInfo_p4() {};
        
  struct SubEvent {
    short		m_time;
    short               m_index;
    short               m_type;
    EventInfo_p3	m_subEventInfo; 
  };

  typedef std::vector<SubEvent>	SubEvVect_t;
  SubEvVect_t		m_subEvents; 
};


#endif


