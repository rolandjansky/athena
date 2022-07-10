/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// Header file for class EventBookkeeper_p1
// Author: David Cote, September 2008. <david.cote@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTBOOKKEEPER_P1_H 
#define EVENTBOOKKEEPER_P1_H 

#include <stdint.h>
#include <string>
#include <vector>

class EventBookkeeper_p1 {
 public: 
  
  ///////////////////////////////////////////////////////////////////////
  //  This class is the persistent form of EventBookkeeper
  ///////////////////////////////////////////////////////////////////////
  
  EventBookkeeper_p1();
  ~EventBookkeeper_p1() = default;

  std::string m_name;
  std::string m_description;
  std::string m_parent;
  std::string m_outputstream;
  std::string m_logic;
  std::vector<std::string> m_childrenEB;
  uint64_t m_nAcceptedEvents = 0;
  double m_nWeightedAcceptedEvents = 0;
  uint64_t m_cycle = 0;
}; 

#endif //> EVENTBOOKKEEPER_P1_H
