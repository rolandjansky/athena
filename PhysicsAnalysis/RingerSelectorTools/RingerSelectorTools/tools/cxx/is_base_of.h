/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: is_base_of.h 667905 2015-05-18 19:07:55Z wsfreund $
#ifndef RINGERSELECTORTOOLS_TOOLS_CXX_ISBASEOF_H
#define RINGERSELECTORTOOLS_TOOLS_CXX_ISBASEOF_H

#include "RingerUseNewCppFeatures.h"

#if RINGER_USE_NEW_CPP_FEATURES
#include<type_traits>
namespace Ringer {
using std::is_base_of;
} // namespace Ringer
#else
#include "boost/type_traits/is_base_of.hpp" 
namespace Ringer {
template<class Base, class Derived>
class is_base_of : public boost::is_base_of<Base,Derived> {};
} // namespace Ringer
#endif

#endif // RINGERSELECTORTOOLS_TOOLS_CXX_ISBASEOF_H
