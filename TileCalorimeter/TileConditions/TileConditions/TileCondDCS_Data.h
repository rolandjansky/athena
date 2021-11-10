/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * TileCondDCS_Data.h
 * @file header file for data object
 * adapted from code written A. Robichaud-Veronneau
 **/

#ifndef TILECONDDCS_DATA_H
#define TILECONDDCS_DATA_H

#include "AthenaKernel/CLASS_DEF.h" 
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include <map>
#include <vector>
#include <string>

class TileCondDCS_Data 
{
public:
  //constructor
 TileCondDCS_Data ();

  //destructor
  virtual ~TileCondDCS_Data();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  /// add defect
  void fill( const CondAttrListCollection::ChanNum& chanNum , const std::string& param);
  /// remove a defect
  void remove( const CondAttrListCollection::ChanNum& chanNum , const std::string& param);
  /// copy all defects to a users vector, the return value is the size
  int output( const CondAttrListCollection::ChanNum & chanNum,std::vector< std::string > & usersVector);

  int output(const CondAttrListCollection::ChanNum & chanNum);

  std::map<CondAttrListCollection::ChanNum, float >* m_pModulesHV;

private:
  typedef std::map<CondAttrListCollection::ChanNum, std::vector< std::string > > DCSConditions;
  DCSConditions m_bad_channels;

};

CLASS_DEF( TileCondDCS_Data , 2911 , 0 )

#endif // TILECONDDCS_DATA_H
