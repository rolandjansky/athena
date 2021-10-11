///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// AthReentrantAlgorithm.cxx 
// Implementation file for class AthReentrantAlgorithm
// Author: C. Leggett
/////////////////////////////////////////////////////////////////// 

// AthenaBaseComps includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthAlgorithmDHUpdate.h"
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/ServiceHandle.h"

// Framework includes
#include "GaudiKernel/ThreadLocalContext.h"

AthReentrantAlgorithm::AthReentrantAlgorithm( const std::string& name, 
                                              ISvcLocator* pSvcLocator ) :
  ::AthCommonDataStore<AthCommonMsg<Gaudi::Algorithm>>   ( name, pSvcLocator )
{

  // Set up to run AthAlgorithmDHUpdate in sysInitialize before
  // merging depedency lists.  This extends the output dependency
  // list with any symlinks implied by inheritance relations.
  m_updateDataHandles =
    std::make_unique<AthenaBaseComps::AthAlgorithmDHUpdate>
    (m_extendedExtraObjects,
     std::move (m_updateDataHandles));
}

// Destructor
///////////////
AthReentrantAlgorithm::~AthReentrantAlgorithm()
{ 
  ATH_MSG_DEBUG ("Calling destructor");
}

/** Specify if the algorithm is clonable
 */
bool AthReentrantAlgorithm::isClonable() const
{
  // Reentrant algorithms are clonable.
  return true;
}

/** Cardinality (Maximum number of clones that can exist)
 *  special value 0 means that algorithm is reentrant
 *
 * Override this to return 0 for reentrant algorithms.
 */
unsigned int AthReentrantAlgorithm::cardinality() const
{
  return 0;
}

/**
 * @brief Execute an algorithm.
 *
 * We override this in order to work around an issue with the Algorithm
 * base class storing the event context in a member variable that can
 * cause crashes in MT jobs.
 */
StatusCode AthReentrantAlgorithm::sysExecute (const EventContext& ctx)
{
  return Gaudi::Algorithm::sysExecute (ctx);
}


/**
 * @brief Return the list of extra output dependencies.
 *
 * This list is extended to include symlinks implied by inheritance
 * relations.
 */
const DataObjIDColl& AthReentrantAlgorithm::extraOutputDeps() const
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
StatusCode AthReentrantAlgorithm::sysInitialize() {
  StatusCode sc=AthCommonDataStore<AthCommonMsg<Gaudi::Algorithm>>::sysInitialize();

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
