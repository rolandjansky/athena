// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_DCSStatCondData.h
 * @brief header file for data object for SCT_DCSConditionsStatCondAlg,
 * SCT_DCSConditionsTool, SCT_Silicon{HV,Temp}CondAlg.
 * @author A. Robichaud-Veronneau
 * @date 23/02/07
 **/

#ifndef SCT_DCSSTATCONDDATA_H
#define SCT_DCSSTATCONDDATA_H

#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include <map>
#include <string>
#include <vector>

/**
 * @class SCT_DCSStatCondData
 * @brief Classs for data object used in SCT_DCSConditionsStatCondAlg,
 * SCT_DCSConditionsTool, SCT_Silicon{HV,Temp}CondAlg.
 **/
class SCT_DCSStatCondData {
public:
  /// Constructor
  SCT_DCSStatCondData();

  /// Destructor
  virtual ~SCT_DCSStatCondData() = default;
  //@name main methods
  //@{
  /// Add defect
  void fill(const CondAttrListCollection::ChanNum& chanNum, const std::string param);
  /// Remove a defect
  void remove(const CondAttrListCollection::ChanNum& chanNum, const std::string param);
  /// Copy all defects to a users vector, the return value is the size
  int output(const CondAttrListCollection::ChanNum& chanNum, std::vector<std::string>& usersVector) const;
  /// Return the number of defects
  int output(const CondAttrListCollection::ChanNum & chanNum) const;
  //@}
  
private:
  typedef std::map<CondAttrListCollection::ChanNum, std::vector<std::string> > DCSConditions;
  DCSConditions m_bad_channels;
};

// Class definition for StoreGate
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_DCSStatCondData , 254074432 , 1 )

// Condition container definition for CondInputLoader
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_DCSStatCondData, 162792902 );

#endif // SCT_DCSSTATCONDDATA_H
