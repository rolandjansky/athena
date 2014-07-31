/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTPCNV_PILEUPEVENTINFO_P1_H
#define EVENTTPCNV_PILEUPEVENTINFO_P1_H
/**
 * @file PileUpEventInfo_p1.h
 *
 * @brief This class is the persistent representation of PileUpEventInfo_p1
 *
 * @author Marcin.Nowak@cern.ch
 */

#include "EventTPCnv/EventInfo_p1.h"
#include <vector>

class MsgStream;

class PileUpEventInfo_p1 : public EventInfo_p1 {
public:

  PileUpEventInfo_p1() {};
  virtual ~PileUpEventInfo_p1() {};
        
  struct SubEvent {
    double		m_timeIndex;
    EventInfo_p1	m_subEventInfo; 
  };

  typedef std::vector<SubEvent>	SubEvVect_t;
  SubEvVect_t		m_subEvents; 
};


#endif


