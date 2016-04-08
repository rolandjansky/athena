/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: is_enum.h 667905 2015-05-18 19:07:55Z wsfreund $
#ifndef RINGERSELECTORTOOLS_TOOLS_CXX_IS_ENUM_H
#define RINGERSELECTORTOOLS_TOOLS_CXX_IS_ENUM_H

#include "RingerUseNewCppFeatures.h"

#if RINGER_USE_NEW_CPP_FEATURES
#include<type_traits>
namespace Ringer {
using std::is_enum;
} // namespace Ringer
#else
#include "boost/type_traits/is_enum.hpp" 
namespace Ringer {
template < class T>
class is_enum : public boost::is_enum<T>{};
} // namespace Ringer
#endif

#endif // RINGERSELECTORTOOLS_TOOLS_CXX_IS_ENUM_H


