/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaBaseComps/src/AthReentrantAlgorithm.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief An algorithm that can be simultaneously executed in multiple threads.
 */


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ThreadLocalContext.h"


/**
 * @brief Standard Gaudi execute method.
 *
 * Find the event context and call @c execute_r.
 */
StatusCode AthReentrantAlgorithm::execute()
{
  return execute_r (Gaudi::Hive::currentContext());
}
