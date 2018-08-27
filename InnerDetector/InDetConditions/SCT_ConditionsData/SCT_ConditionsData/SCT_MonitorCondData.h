/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_MonitorCondData.h
 * header file for data object
 * @author Susumu Oda - 2017-07-26
 **/

#ifndef SCT_MONITORCONDDATA_H
#define SCT_MONITORCONDDATA_H

#include <map>
#include <string>

// Include Athena stuff
#include "AthenaKernel/CLASS_DEF.h"

class SCT_MonitorCondData {
public:

  // Constructor
  SCT_MonitorCondData();

  // Destructor
  virtual ~SCT_MonitorCondData();

  // Check if a module has a defect (a list of bad strips). If it does not have defect return false.
  bool find(const int& channelNumber, std::string& defectList) const;

  // Insert a new defect (a list of bad strips) for a module
  void insert(const int& channelNumber, const std::string& defectList);

  // Clear m_defectListMap
  void clear();

private:
  // Store the relation between modules and defects (lists of bad strips).
  std::map<const int, const std::string> m_defectListMap;

};

CLASS_DEF( SCT_MonitorCondData , 190515334 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_MonitorCondData, 164599336 );


#endif // SCT_MONITORCONDDATA_H
