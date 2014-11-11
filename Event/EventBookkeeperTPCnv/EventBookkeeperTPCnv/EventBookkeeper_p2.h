/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// Header file for class EventBookkeeper_p2
// Author: David Cote, September 2008. <david.cote@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTBOOKKEEPER_p2_H 
#define EVENTBOOKKEEPER_p2_H 

#include <stdint.h>
#include <string>
#include <vector>

class EventBookkeeper_p2 {
 public: 
  
  ///////////////////////////////////////////////////////////////////////
  //  This class is the persistent form of EventBookkeeper
  ///////////////////////////////////////////////////////////////////////
  
  EventBookkeeper_p2();
  EventBookkeeper_p2(const EventBookkeeper_p2& rhs);
  EventBookkeeper_p2& operator=(const EventBookkeeper_p2& rhs);
  ~EventBookkeeper_p2();
  
  std::string m_name;
  std::string m_description;
  std::string m_inputstream;
  std::string m_outputstream;
  std::string m_logic;
  std::vector<EventBookkeeper_p2*>* m_childrenEB;
  unsigned long long m_nAcceptedEvents;
  double m_nWeightedAcceptedEvents;
  int m_cycle;
}; 

#endif //> EVENTBOOKKEEPER_p2_H
