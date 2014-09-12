/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/src/AuxStoreStandalone.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2013
 * @brief Auxiliary data store for standalone objects.
 */


#include "AthContainers/AuxStoreStandalone.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/tools/error.h"
#include <sstream>


namespace SG {


/**
 * @brief Constructor.
 */
AuxStoreStandalone::AuxStoreStandalone()
  : AuxStoreInternal (true)
{
}


} // namespace SG
