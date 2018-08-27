///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthReentrantAlgorithm.cxx 
// Implementation file for class AthReentrantAlgorithm
// Author: C. Leggett
/////////////////////////////////////////////////////////////////// 

// AthenaBaseComps includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthAlgorithmDHUpdate.h"

// Framework includes
#include "GaudiKernel/ThreadLocalContext.h"

AthReentrantAlgorithm::AthReentrantAlgorithm( const std::string& name, 
			    ISvcLocator* pSvcLocator,
			    const std::string& version ) : 
  ::AthCommonDataStore<AthCommonMsg<ReEntAlgorithm>>   ( name, pSvcLocator, version )
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

#ifndef REENTRANT_GAUDI
/**
 * @brief Standard Gaudi execute method.
 *
 * Find the event context and call @c execute_r.
 */
StatusCode AthReentrantAlgorithm::execute()
{
  return execute_r (Gaudi::Hive::currentContext());
}
#endif


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
 * @brief Return the current event context.
 *
 * Override this because the base class version won't work correctly
 * for reentrant algorithms.  (We shouldn't really be using this
 * for reentrant algorithms, but just in case.).
 */
const EventContext& AthReentrantAlgorithm::getContext() const
{
  return Gaudi::Hive::currentContext();
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
  EventContext ctx2 = ctx;
  // sysExecute will assign the context we pass in to a member variable of 
  // the algorithm.  If the context is referencing any dynamic memory,
  // then we can end up with a double-delete.  So clear out any extension ---
  // we won't actually use the context we pass to here for anything anyway.
  ctx2.setExtension (boost::any());
  return ::ReEntAlgorithm::sysExecute (ctx2);
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
