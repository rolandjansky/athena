/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: enable_if.h 693383 2015-09-06 22:08:55Z wsfreund $
#ifndef RINGERSELECTORTOOLS_TOOLS_CXX_ENABLE_IF_H
#define RINGERSELECTORTOOLS_TOOLS_CXX_ENABLE_IF_H

#include "RingerUseNewCppFeatures.h"


#if RINGER_USE_NEW_CPP_FEATURES
#include<type_traits>
namespace Ringer {
using std::enable_if;
} // namespace Ringer
#else
#include "CxxUtils/enable_if.h" 
namespace Ringer {
template<bool Cond, class T = void>
class enable_if : public enable_if<Cond,T> {};
} // namespace Ringer
#endif

#endif // RINGERSELECTORTOOLS_TOOLS_CXX_ENABLE_IF_H


