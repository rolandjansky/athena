/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthAllocators/src/exceptions.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2016
 * @brief Exceptions that can be thrown from AthAllocators.
 */


#include "AthAllocators/exceptions.h"
#include <sstream>


namespace SG {


//*************************************************************************


/**
 * @brief Constructor.
 */
ExcDifferentArenas::ExcDifferentArenas()
  : std::runtime_error ("Attempt to assign between ArenaSharedHeapSTLAllocators for different arenas.")
{
}


} // namespace SG
