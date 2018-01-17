/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * SCT_ModuleVetoCondData.h
 * @file header file for data object
 * @author Susumu Oda - 04/12/17
 **/

#ifndef SCT_MODULEVETOCONDDATA_H
#define SCT_MODULEVETOCONDDATA_H

#include "Identifier/Identifier.h"
#include <set>

class SCT_ModuleVetoCondData {
public:
  /// constructor
  SCT_ModuleVetoCondData();
  /// destructor
  virtual ~SCT_ModuleVetoCondData();
  /// set a bad wafer ID
  bool setBadWaferId(const Identifier waferId);
  /// check if a wafer ID is bad or not
  bool isBadWaferId(const Identifier waferId) const; 
  /// clear
  void clear();
  /// set data filled
  void setFilled();
  /// check the data are filled or not
  bool filled() const;
  /// get the number of bad wafers
  long unsigned int size() const;
  
private:
  std::set<Identifier> m_badWaferId;
  bool m_isFilled;
};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_ModuleVetoCondData , 100510855 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_ModuleVetoCondData, 46782241 );

#endif // SCT_MODULEVETOCONDDATA_H
