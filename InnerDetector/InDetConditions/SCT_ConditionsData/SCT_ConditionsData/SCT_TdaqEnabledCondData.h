/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_TdaqEnabledCondData.h
 * header file for data object
 * @author Susumu Oda - 2017-05-15
 **/

#ifndef SCT_TDAQENABLEDCONDDATA_H
#define SCT_TDAQENABLEDCONDDATA_H

#include <set>
#include <vector>

// Include Athena stuff
#include "CLIDSvc/CLASS_DEF.h"
#include "Identifier/IdentifierHash.h"

class SCT_TdaqEnabledCondData {
public:

  // Constructor
  SCT_TdaqEnabledCondData();

  // Destructor
  virtual ~SCT_TdaqEnabledCondData();

  // Set a good ROD
  bool setGoodRod(const unsigned int rodNumber);

  // Get good RODs
  const std::set<unsigned int>& getGoodRods() const;

  // Set a list of good modules
  void setGoodModules(const std::vector<IdentifierHash>& idVec);

  // Set filled variable
  void setFilled(const bool filled);

  // Get filled variable
  bool isFilled() const;

  // Set noneBad value
  void setNoneBad(const bool noneBad);

  // Get noneBad value
  bool isNoneBad() const;

  // Check if a module is good
  bool isGood(const IdentifierHash& hashId) const;

  // Clear m_goodRods, m_goodIds, m_noneBad, m_filled
  void clear();

private:

  std::set<unsigned int> m_goodRods;
  std::set<IdentifierHash> m_goodIds;
  bool m_noneBad;
  bool m_filled;

};

CLASS_DEF( SCT_TdaqEnabledCondData , 137549585 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_TdaqEnabledCondData , 31730865 );


#endif // SCT_TDAQENABLEDDATA_H
