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

// Include Athena stuff
#include "AthenaKernel/CLASS_DEF.h"
#include "Identifier/IdentifierHash.h"

#include <array>
#include <string>

class SCT_MonitorCondData {
public:

  // Constructor
  SCT_MonitorCondData();

  // Destructor
  virtual ~SCT_MonitorCondData() = default;

  // Check if a module has a defect (a list of bad strips). If it does not have defect return false.
  bool find(const IdentifierHash& hash, std::string& defectList) const;

  // Insert a new defect (a list of bad strips) for a module
  void insert(const IdentifierHash& hash, const std::string& defectList);

  // Clear m_defectListArray
  void clear();

private:
  enum {N_MODULES=4088};
  // Store the relation between modules and defects (lists of bad strips).
  std::array<std::string, N_MODULES> m_defectListArray;

};

CLASS_DEF( SCT_MonitorCondData , 190515334 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_MonitorCondData, 164599336 );


#endif // SCT_MONITORCONDDATA_H
