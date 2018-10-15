/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthAlgTool.h 
// Header file for class AthAlgTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENABASECOMPS_ATHALGTOOL_H 
#define ATHENABASECOMPS_ATHALGTOOL_H 1

// STL includes
#include <string>
#include <type_traits>


// Framework includes
#include "AthenaBaseComps/AthCommonDataStore.h"
#include "AthenaBaseComps/AthCommonMsg.h"
#include "AthenaBaseComps/AthMemMacros.h"

#include "GaudiKernel/AlgTool.h"

class AthAlgTool : 
  public AthCommonDataStore<AthCommonMsg<AlgTool>>
{ 

public: 

  /// Constructor with parameters: 
  AthAlgTool( const std::string& type,
              const std::string& name, 
              const IInterface* parent );

  /// Destructor: 
  virtual ~AthAlgTool(); 

private: 

  /// Default constructor: 
  AthAlgTool(); //> not implemented
  AthAlgTool (const AthAlgTool&); //> not implemented
  AthAlgTool& operator= (const AthAlgTool&); //> not implemented

}; 

#endif //> ATHENABASECOMPS_ATHALGTOOL_H
