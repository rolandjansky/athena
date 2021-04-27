///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// AthAlgorithm.cxx 
// Implementation file for class AthAlgorithm
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AthenaBaseComps includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthAlgorithmDHUpdate.h"
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/ServiceHandle.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
AthAlgorithm::AthAlgorithm( const std::string& name, 
                            ISvcLocator* pSvcLocator ) :
  ::AthCommonDataStore<AthCommonMsg<Algorithm>>   ( name, pSvcLocator )
{
  // Set up to run AthAlgorithmDHUpdate in sysInitialize before
  // merging dependency lists.  This extends the output dependency
  // list with any symlinks implied by inheritance relations.
  m_updateDataHandles =
    std::make_unique<AthenaBaseComps::AthAlgorithmDHUpdate>
    (m_extendedExtraObjects,
     std::move (m_updateDataHandles));
}

// Destructor
///////////////
AthAlgorithm::~AthAlgorithm()
{ 
}


/**
 * @brief Return the list of extra output dependencies.
 *
 * This list is extended to include symlinks implied by inheritance
 * relations.
 */
const DataObjIDColl& AthAlgorithm::extraOutputDeps() const
{
  // If we didn't find any symlinks to add, just return the collection
  // from the base class.  Otherwise, return the extended collection.
  if (!m_extendedExtraObjects.empty()) {
    return m_extendedExtraObjects;
  }
  return Algorithm::extraOutputDeps();
}

/**
 * @brief Override sysInitialize from the base class
 *
 * Scan through all outputHandles, and if they're WriteCondHandles,
 * register them with the CondSvc
 */
StatusCode AthAlgorithm::sysInitialize() {
  StatusCode sc=AthCommonDataStore<AthCommonMsg<Algorithm>>::sysInitialize();

  if (sc.isFailure()) {
    return sc;
  }
  ServiceHandle<ICondSvc> cs("CondSvc",name());
  for (auto h : outputHandles()) {
    if (h->isCondition() && h->mode() == Gaudi::DataHandle::Writer) {
      // do this inside the loop so we don't create the CondSvc until needed
      if ( cs.retrieve().isFailure() ) {
        ATH_MSG_WARNING("no CondSvc found: won't autoreg WriteCondHandles");
        return StatusCode::SUCCESS;
      }
      if (cs->regHandle(this,*h).isFailure()) {
        sc = StatusCode::FAILURE;
        ATH_MSG_ERROR("unable to register WriteCondHandle " << h->fullKey()
                      << " with CondSvc");
      }
    }
  }
  return sc;
}
