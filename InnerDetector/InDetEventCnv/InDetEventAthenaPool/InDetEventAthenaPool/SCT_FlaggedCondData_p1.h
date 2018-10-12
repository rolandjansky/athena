/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_FLAGGEDCONDDATA_P1_H
#define SCT_FLAGGEDCONDDATA_P1_H

#include <map>
#include <string>

#include "Identifier/IdentifierHash.h"

class SCT_FlaggedCondData_p1 {
 public:
  SCT_FlaggedCondData_p1(): m_data{} {};
  // List of Cnv classes that convert this into Rdo objects
  friend class SCT_FlaggedCondDataCnv_p1;
 private:
  std::map<IdentifierHash::value_type, std::string> m_data;
};

#endif // SCT_FLAGGEDCONDDATA_P1_H
