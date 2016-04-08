/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: add_lvalue_reference.h 667905 2015-05-18 19:07:55Z wsfreund $
#ifndef RINGERSELECTORTOOLS_TOOLS_CXX_ADD_LVALUE_REFERENCE_H
#define RINGERSELECTORTOOLS_TOOLS_CXX_ADD_LVALUE_REFERENCE_H

#include "RingerUseNewCppFeatures.h"

#if RINGER_USE_NEW_CPP_FEATURES
#include<type_traits>
namespace Ringer {
using std::add_lvalue_reference;
} // namespace Ringer
#else
#include "boost/type_traits/add_lvalue_reference.hpp" 
namespace Ringer {
template < class T>
class add_lvalue_reference : public boost::add_lvalue_reference<T>{};
} // namespace Ringer
#endif

#endif // RINGERSELECTORTOOLS_TOOLS_CXX_ADD_LVALUE_REFERENCE_H


