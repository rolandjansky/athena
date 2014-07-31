/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTPCNV_TRIGGERINFO_P1_H
#define EVENTTPCNV_TRIGGERINFO_P1_H
/**
 * @file TriggerInfo_p1.h
 *
 * @brief This class is the persistent representation of TriggerInfo
 *
 * @author Marcin.Nowak@cern.ch
 */

#include <vector>

class TriggerInfo_p1 {
public:
  typedef unsigned int number_type;
  TriggerInfo_p1() 
    : m_extendedLevel1ID(0),
      m_level1TriggerType(0),
      m_level2TriggerInfo(0)
  {}

  number_type               m_extendedLevel1ID;
  number_type               m_level1TriggerType;
  number_type               m_level2TriggerInfo;
  std::vector<number_type>  m_eventFilterInfo;
};


#endif 
