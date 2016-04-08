/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: StaticAssert.h 667905 2015-05-18 19:07:55Z wsfreund $
#ifndef RINGERSELECTORTOOLS_TOOLS_CXX_STATICASSERT_H
#define RINGERSELECTORTOOLS_TOOLS_CXX_STATICASSERT_H

#include "RingerUseNewCppFeatures.h"

#undef RINGER_STATIC_ASSERT

#if RINGER_USE_NEW_CPP_FEATURES
#include<type_traits>
#define RINGER_STATIC_ASSERT(cond,msg) static_assert(cond,msg)
#else
#include "boost/static_assert.hpp"
#define RINGER_STATIC_ASSERT(cond,msg) BOOST_STATIC_ASSERT_MSG(cond,msg)
#endif

#endif // RINGERSELECTORTOOLS_TOOLS_CXX_STATICASSERT_H
