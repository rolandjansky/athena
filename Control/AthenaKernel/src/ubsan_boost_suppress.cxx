/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

// $Id$
/**
 * @file AthenaKernel/src/ubsan_boost_suppress.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2017
 * @brief Suppress some known ubsan warnings from boost.
 */


#include "CxxUtils/ubsan_suppress.h"
#include "boost/format.hpp"


namespace Athena {


int ubsan_boost_suppress()
{
  // See <https://svn.boost.org/trac/boost/ticket/11632>
  CxxUtils::ubsan_suppress ([]() { boost::format("%1%") % "asd"; });
  return 0;
}


int ubsan_boost_suppress_ = ubsan_boost_suppress();


} // namespace Athena
