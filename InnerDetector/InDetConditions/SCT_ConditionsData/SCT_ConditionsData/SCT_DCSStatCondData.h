/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * SCT_DCSStatCondData.h
 * @file header file for data object
 * @author A. Robichaud-Veronneau - 23/02/07
 **/

#ifndef SCT_DCSSTATCONDDATA_H
#define SCT_DCSSTATCONDDATA_H

#include "CLIDSvc/CLASS_DEF.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include <map>
#include <vector>
#include <string>

class SCT_DCSStatCondData {
public:
  //constructor
  SCT_DCSStatCondData();

  //destructor
  virtual ~SCT_DCSStatCondData();
  //@name main methods
  //@{
  /// add defect
  void fill(const CondAttrListCollection::ChanNum& chanNum, const std::string param);
  /// remove a defect
  void remove(const CondAttrListCollection::ChanNum& chanNum, const std::string param);
  /// copy all defects to a users vector, the return value is the size
  int output(const CondAttrListCollection::ChanNum& chanNum, std::vector<std::string>& usersVector) const;
  ///
  int output(const CondAttrListCollection::ChanNum & chanNum) const;
  //@}
  
private:
  typedef std::map<CondAttrListCollection::ChanNum, std::vector<std::string> > DCSConditions;
  DCSConditions m_bad_channels;
};

CLASS_DEF( SCT_DCSStatCondData , 254074432 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_DCSStatCondData, 162792902 );


#endif // SCT_DCSSTATCONDDATA_H
