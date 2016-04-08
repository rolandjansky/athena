/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: remove_cv.h 667905 2015-05-18 19:07:55Z wsfreund $
#ifndef RINGERSELECTORTOOLS_TOOLS_CXX_REMOVE_CV_H
#define RINGERSELECTORTOOLS_TOOLS_CXX_REMOVE_CV_H

#include "RingerUseNewCppFeatures.h"

#if RINGER_USE_NEW_CPP_FEATURES
#include<type_traits>
namespace Ringer {
using std::remove_cv;
} // namespace Ringer
#else
#include "boost/type_traits/remove_cv.hpp" 
namespace Ringer {
template < class T>
class remove_cv : public boost::remove_cv<T>{};
} // namespace Ringer
#endif

#endif // RINGERSELECTORTOOLS_TOOLS_CXX_REMOVE_CV_H

