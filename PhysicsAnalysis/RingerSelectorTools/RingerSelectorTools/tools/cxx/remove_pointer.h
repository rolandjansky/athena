/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: remove_pointer.h 667905 2015-05-18 19:07:55Z wsfreund $
#ifndef RINGERSELECTORTOOLS_TOOLS_CXX_REMOVE_POINTER_H
#define RINGERSELECTORTOOLS_TOOLS_CXX_REMOVE_POINTER_H

#include "RingerUseNewCppFeatures.h"

#if RINGER_USE_NEW_CPP_FEATURES
#include<type_traits>
namespace Ringer {
using std::remove_pointer;
} // namespace Ringer
#else
#include "boost/type_traits/remove_pointer.hpp" 
namespace Ringer {
template < class T>
class remove_pointer : public boost::remove_pointer<T>{};
} // namespace Ringer
#endif

#endif // RINGERSELECTORTOOLS_TOOLS_CXX_REMOVE_POINTER_H


