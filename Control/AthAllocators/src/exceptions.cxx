/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthAllocators/src/exceptions.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2016
 * @brief Exceptions that can be thrown from AthAllocators.
 */

#include "AthAllocators/exceptions.h"
#include <sstream>
#include <string.h>


namespace SG {


//*************************************************************************


/**
 * @brief Constructor.
 */
ExcDifferentArenas::ExcDifferentArenas()
  : std::runtime_error ("SG::ExcDifferentAreas: Attempt to assign between ArenaSharedHeapSTLAllocators for different arenas.")
{
}


//*************************************************************************


std::string excProtection_format (int errnum)
{
  std::ostringstream os;
  os << "SG::ExcProtection: Attempt to change memory protection failed: ";
  char errbuf[256];
#ifdef _GNU_SOURCE
  os << strerror_r (errnum, errbuf, sizeof(errbuf));
#else
  strerror_r (errnum, errbuf, sizeof(errbuf));
  os << errbuf;
#endif
  return os.str();
}


/**
 * @brief Constructor.
 * @param errnum The system error code.
 */
ExcProtection::ExcProtection (int errnum)
  : std::runtime_error (excProtection_format (errnum))
{
}


//*************************************************************************


/**
 * @brief Constructor.
 */
ExcProtected::ExcProtected()
  : std::runtime_error ("SG::ExcProtected: Attempt to change protected arena.")
{
}


} // namespace SG
