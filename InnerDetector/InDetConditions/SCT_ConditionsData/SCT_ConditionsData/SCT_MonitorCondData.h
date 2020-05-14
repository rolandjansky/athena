// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_MonitorCondData.h
 * @brief header file for data object for SCT_MonitorCondAlg and SCT_MonitorConditionsTool.
 * @author Susumu Oda
 * @date 2017-07-26
 **/

#ifndef SCT_MONITORCONDDATA_H
#define SCT_MONITORCONDDATA_H

// Include Athena stuff
#include "Identifier/IdentifierHash.h"
#include "SCT_ConditionsData/SCT_ConditionsParameters.h"

#include <array>
#include <bitset>
#include <string>

/**
 * @class SCT_MonitorCondData
 * @brief Class for data object used in SCT_MonitorCondAlg and SCT_MonitorConditionsTool.
 **/
class SCT_MonitorCondData {
public:

  /// Constructor
  SCT_MonitorCondData();

  /// Destructor
  virtual ~SCT_MonitorCondData() = default;

  /// Check if a module has a defect (a list of bad strips). If it does not have defect return false.
  bool find(const IdentifierHash& hash, std::string& defectList) const;

  /// Insert a new defect (a list of bad strips) for a module
  void insert(const IdentifierHash& hash, const std::string& defectList);

  /// Clear m_defectListArray
  void clear();

  /// Get the number of bad strips for a module
  std::size_t nBadStripsForModule(const IdentifierHash& moduleHash) const;
  /// Get the number of bad strips for a wafer
  std::size_t nBadStripsForWafer(const IdentifierHash& waferHash) const;
  /// Get the number of bad strips for the chip, where a strip locates
  std::size_t nBadStripsForChip(const IdentifierHash& waferHash, const int strip) const;
  /// Check if a strip is bad
  bool isBadStrip(const IdentifierHash& waferHash, const int strip) const;

private:
  void clearModule(const std::size_t moduleIndex);
  void fillBadStrip(const std::size_t moduleIndex, const int strip);

  /// Store the relation between modules and defects (lists of bad strips).
  std::array<std::string, SCT_ConditionsData::NUMBER_OF_MODULES> m_defectListArray;
  std::array<std::array<std::array<std::bitset<SCT_ConditionsData::STRIPS_PER_CHIP>,
    SCT_ConditionsData::CHIPS_PER_SIDE>,
    SCT_ConditionsData::SIDES_PER_MODULE>,
    SCT_ConditionsData::NUMBER_OF_MODULES> m_badStripArray;
  std::array<std::array<std::array<std::size_t,
    SCT_ConditionsData::CHIPS_PER_SIDE>,
    SCT_ConditionsData::SIDES_PER_MODULE>,
    SCT_ConditionsData::NUMBER_OF_MODULES> m_badChipArray;
  std::array<std::array<std::size_t,
    SCT_ConditionsData::SIDES_PER_MODULE>,
    SCT_ConditionsData::NUMBER_OF_MODULES> m_badWaferArray;
  std::array<std::size_t,
    SCT_ConditionsData::NUMBER_OF_MODULES> m_badModuleArray;

  static const std::string s_separator;
};

// Class definition for StoreGate
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_MonitorCondData , 190515334 , 1 )

// Condition container definition for CondInputLoader
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_MonitorCondData, 164599336 );

#endif // SCT_MONITORCONDDATA_H
