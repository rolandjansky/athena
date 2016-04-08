/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: decay.h 667905 2015-05-18 19:07:55Z wsfreund $
#ifndef RINGERSELECTORTOOLS_TOOLS_CXX_DECAY_H
#define RINGERSELECTORTOOLS_TOOLS_CXX_DECAY_H

#include "RingerUseNewCppFeatures.h"

#if RINGER_USE_NEW_CPP_FEATURES
#include<type_traits>
namespace Ringer {
using std::decay;
} // namespace Ringer
#else
#include "boost/type_traits/decay.hpp" 
namespace Ringer {
template < class T >
class decay : public boost::decay<T>{};
} // namespace Ringer
#endif

#endif // RINGERSELECTORTOOLS_TOOLS_CXX_DECAY_H

