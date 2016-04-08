/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: is_same.h 693383 2015-09-06 22:08:55Z wsfreund $
#ifndef RINGERSELECTORTOOLS_TOOLS_CXX_ISSAME_H
#define RINGERSELECTORTOOLS_TOOLS_CXX_ISSAME_H

#include "RingerUseNewCppFeatures.h"

#if RINGER_USE_NEW_CPP_FEATURES
#include<type_traits>
namespace Ringer {
using std::is_same;
} // namespace Ringer
#else
#include "boost/type_traits/is_base_of.hpp" 
namespace Ringer {
template < class T, class U>
class is_same : public boost::is_same<T,U>{};
} // namespace Ringer
#endif

#endif // RINGERSELECTORTOOLS_TOOLS_CXX_ISSAME_H
