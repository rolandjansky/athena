/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// AthAlgorithm.h 
// Header file for class AthAlgorithm
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENABASECOMPS_ATHALGORITHM_H
#define ATHENABASECOMPS_ATHALGORITHM_H 1

// STL includes
#include <string>
#include <type_traits>


// Framework includes
#include "AthenaBaseComps/AthCommonDataStore.h"
#include "AthenaBaseComps/AthCommonMsg.h"
#include "AthenaBaseComps/AthMemMacros.h"
#include "GaudiKernel/Algorithm.h"

/** @class AthAlgorithm AthAlgorithm.h AthenaBaseComps/AthAlgorithm.h
 *
 *  Base class from which all concrete Athena algorithm classes should 
 *  be derived. 
 *  In order for a concrete algorithm class to do anything
 *  useful the methods initialize(), execute() and finalize() 
 *  should be overridden.
 *  The base class provides utility methods for accessing 
 *  standard services (StoreGate service etc.); for declaring
 *  properties which may be configured by the job options 
 *  service; and for creating sub algorithms.
 *  The only base class functionality which may be used in the
 *  constructor of a concrete algorithm is the declaration of 
 *  member variables as properties. All other functionality, 
 *  i.e. the use of services and the creation of sub-algorithms,
 *  may be used only in initialize() and afterwards (see the 
 *  Gaudi and Athena user guides).
 *
 *  @author Sebastien Binet
 *  @date   2008
 */ 

class AthAlgorithm 
  : public AthCommonDataStore<AthCommonMsg< Algorithm >>
{ 
 public: 

  /// Constructor with parameters: 
  AthAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor: 
  virtual ~AthAlgorithm(); 

  /** @brief Override sysInitialize
   *
   * Loop through all output handles, and if they're WriteCondHandles,
   * automatically register them and this Algorithm with the CondSvc
   */
  virtual StatusCode sysInitialize() override;
  
  /**
   * @brief Return the list of extra output dependencies.
   *
   * This list is extended to include symlinks implied by inheritance
   * relations.
   */
  virtual const DataObjIDColl& extraOutputDeps() const override;

  
 private: 

  /// Default constructor: 
  AthAlgorithm(); //> not implemented
  AthAlgorithm (const AthAlgorithm& ); //> not implemented
  AthAlgorithm& operator= (const AthAlgorithm&); //> not implemented

  DataObjIDColl m_extendedExtraObjects;

}; 

#endif //> !ATHENABASECOMPS_ATHALGORITHM_H
