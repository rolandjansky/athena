/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

///////////////////////////////////////////////////////////////////
// SharedObject.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUITLS_SHAREDOBJECT_H
#define TRKDETDESCRUITLS_SHAREDOBJECT_H
#include <memory>

namespace Trk {

/** @class SharedObject

  typedef for shared_ptr.
  Here just to ease migrations of clients
  to C++11 style smart ptr with thread safe
  reference counting.
  */

template<class T>
using SharedObject = std::shared_ptr<T>;

template<typename T>
const auto do_not_delete = [](T*) {};

} // end of namespace

#endif // TRKDETDESCRUITLS_SHAREDOBJECT_H
