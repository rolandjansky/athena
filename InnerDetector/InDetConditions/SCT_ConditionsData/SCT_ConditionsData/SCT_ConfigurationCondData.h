/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConfigurationCondData.h
 * header file for data object
 * @author Susumu Oda - 2017-10-10
 **/

#ifndef SCT_CONFIGURATIONCONDDATA_H
#define SCT_CONFIGURATIONCONDDATA_H

#include <array>
#include <bitset>
#include <map>
#include <set>

// Include Athena stuff
#include "AthenaKernel/CLASS_DEF.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

class SCT_ConfigurationCondData {
public:

  // Constructor
  SCT_ConfigurationCondData();

  // Destructor
  virtual ~SCT_ConfigurationCondData() = default;

  // Set a bad strip identifier
  void setBadStripId(const Identifier& badStripId, const IdentifierHash& hash, const int strip);
  // Get all bad strip identifiers
  const std::set<Identifier>* getBadStripIds() const;
  // Clear all bad strip identifiers
  void clearBadStripIds();
  // Check if a strip identifier is bad one
  bool isBadStrip(const IdentifierHash& hash, const int strip) const;

  // Set a bad wafer identifier
  void setBadWaferId(const Identifier& badWaferId);
  // Get all bad wafer identifiers
  const std::set<Identifier>* getBadWaferIds() const;
  // Clear all bad wafer identifiers
  void clearBadWaferIds();
  // Check if a wafer identifier is bad one
  bool isBadWaferId(const Identifier& waferId) const;

  // Set a bad module identifier
  void setBadModuleId(const Identifier& badModuleId);
  // Get all bad module identifiers
  const std::set<Identifier>* getBadModuleIds() const;
  // Clear all bad module identifiers
  void clearBadModuleIds();
  // Check if a module identifier is bad one
  bool isBadModuleId(const Identifier& moduleId) const;

  // Set bad links for a module
  void setBadLinks(const IdentifierHash& hash, const bool isBadLink0, const bool isBadLink1);
  // Get all bad links
  const std::map<IdentifierHash, std::pair<bool, bool>>* getBadLinks() const;
  // Clear all bad links
  void clearBadLinks();
  // Check if a module has bad links
  std::pair<bool, bool> areBadLinks(const IdentifierHash& hash) const;

  // Set bad chips for a module
  void setBadChips(const Identifier& moduleId, const unsigned int chipStatus);
  // Get bad chips for a module
  unsigned int getBadChips(const Identifier& moduleId) const;
  // Get all bad chips
  const std::map<Identifier, unsigned int>* getBadChips() const;
  // Clear all bad chips
  void clearBadChips();

  // Clear all bad information
  void clear();

private:
  enum {N_MODULES=4088, N_STRIPS=768, N_SIDES=2};

  std::set<Identifier> m_badStripIds;
  std::array<std::bitset<N_STRIPS>, N_MODULES*N_SIDES> m_badStripArray;
  std::set<Identifier> m_badWaferIds;
  std::set<Identifier> m_badModuleIds;
  std::map<IdentifierHash, std::pair<bool, bool>> m_badLinks;
  std::array<std::pair<bool, bool>, N_MODULES> m_badLinksArray;
  std::map<Identifier, unsigned int> m_badChips;
};

CLASS_DEF( SCT_ConfigurationCondData , 115061956 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_ConfigurationCondData, 23220694 );


#endif // SCT_CONFIGURATIONDATA_H
