// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ModuleVetoCondData.h
 * @brief header file for data object for SCT_ModuleVetoCondAlg,
 * SCT_LinkMaskingCondAlg, SCT_ModuleVetoTool, SCT_LinkMaskingTool.
 * @author Susumu Oda
 * @date 04/12/17
 **/

#ifndef SCT_MODULEVETOCONDDATA_H
#define SCT_MODULEVETOCONDDATA_H

#include "Identifier/Identifier.h"

#include <set>

/**
 * @class SCT_ModuleVetoCondData
 * @brief Class for data object used in SCT_ModuleVetoCondAlg,
 * SCT_LinkMaskingCondAlg, SCT_ModuleVetoTool, SCT_LinkMaskingTool.
 **/
class SCT_ModuleVetoCondData {
public:
  /// Constructor
  SCT_ModuleVetoCondData();
  /// Destructor
  virtual ~SCT_ModuleVetoCondData() = default;
  /// Set a bad wafer ID
  bool setBadWaferId(const Identifier waferId);
  /// Check if a wafer ID is bad or not
  bool isBadWaferId(const Identifier waferId) const; 
  /// Clear
  void clear();
  /// Set data filled
  void setFilled();
  /// Check the data are filled or not
  bool filled() const;
  /// Get the number of bad wafers
  long unsigned int size() const;
  
private:
  std::set<Identifier> m_badWaferId;
  bool m_isFilled;
};

// Class definition for StoreGate
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_ModuleVetoCondData , 100510855 , 1 )

// Condition container definition for CondInputLoader
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_ModuleVetoCondData, 46782241 );

#endif // SCT_MODULEVETOCONDDATA_H
