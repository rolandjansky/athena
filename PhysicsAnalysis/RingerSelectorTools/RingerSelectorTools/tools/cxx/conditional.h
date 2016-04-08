/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: conditional.h 667905 2015-05-18 19:07:55Z wsfreund $
#ifndef RINGERSELECTORTOOLS_TOOLS_CXX_CONDITIONAL_H
#define RINGERSELECTORTOOLS_TOOLS_CXX_CONDITIONAL_H

#include "RingerUseNewCppFeatures.h"

#if RINGER_USE_NEW_CPP_FEATURES
#include<type_traits>
namespace Ringer {
using std::conditional;
} // namespace Ringer
#else
#include "boost/type_traits/conditional.hpp" 
namespace Ringer {
template<bool Cond, class T, class F>
class conditional : public boost::conditional<Cond,T,F> {};
} // namespace Ringer
#endif

#endif // RINGERSELECTORTOOLS_TOOLS_CXX_CONDITIONAL_H

