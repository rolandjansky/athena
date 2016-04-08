/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * SCT_DCSConditionsData.h
 * @file header file for data object
 * @author A. Robichaud-Veronneau - 23/02/07
 **/

#ifndef SCT_DCSCONDITIONSDATA_H
#define SCT_DCSCONDITIONSDATA_H

#include "CLIDSvc/CLASS_DEF.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include <map>
#include <vector>
#include <string>

class SCT_DCSConditionsData {
public:
  //constructor
  SCT_DCSConditionsData();

  //destructor
  virtual ~SCT_DCSConditionsData();
  //@name main methods
  //@{
  /// add defect
  void fill( const CondAttrListCollection::ChanNum& chanNum , const std::string param);
  /// remove a defect
  void remove( const CondAttrListCollection::ChanNum& chanNum , const std::string param);
  /// copy all defects to a users vector, the return value is the size
  int output( const CondAttrListCollection::ChanNum & chanNum,std::vector< std::string > & usersVector);
  //@}
  
  //@name methods re-instated to satisfy 14.2.2X-VAL compilation
  //@{
  void fill( const CondAttrListCollection::ChanNum& chanNum , const int status);//!< dummy, to fix rel 14.2 
  bool check( const CondAttrListCollection::ChanNum& chanNum , const int status); //!< dummy, to fix rel 14.2 
  int output(const CondAttrListCollection::ChanNum & chanNum);
  //@}
  
private:
  typedef std::map<CondAttrListCollection::ChanNum, std::vector< std::string > > DCSConditions;
  DCSConditions m_bad_channels;
};

CLASS_DEF( SCT_DCSConditionsData , 91615746 , 1 )

#endif // SCT_DCSCONDITIONSDATA_H
