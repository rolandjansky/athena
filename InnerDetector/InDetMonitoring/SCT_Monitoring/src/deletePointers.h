/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file deletePointers.h
 * @author Shaun Roe
 * @date 1/09/2009
 * Header file for declaration and definition of templated functions to
 * delete a pointer and a container of pointers.
 * The template may be further specialized to delete a map of pointers (not done here)
 *
 **/
#ifndef SCTMonitoring_deletePointers_h
#define SCTMonitoring_deletePointers_h
#include <algorithm>
namespace SCT_Monitoring {
///Delete a pointer and set it to zero. Suitable for use in a 'std::for_each' algorithm
// @todo check the type is really a pointer
  template <class PointerType>
  void
  deletePointer(PointerType &p) {
    delete p;
    p = 0;
  }

///Delete a container of pointers and then clear it
  template<class C>
  void
  deletePtrArray(C &container) {
    std::for_each(container.begin(), container.end(), deletePointer<typename C::value_type>);

    container.clear();
  }
}// end of namespace

#endif