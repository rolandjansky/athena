// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaKernel/DataObjectSharedPtr.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Smart pointer to manage @c DataObject reference counts.
 */


#ifndef ATHENAKERNEL_DATAOBJECTSHAREDPTR_H
#define ATHENAKERNEL_DATAOBJECTSHAREDPTR_H


#include "GaudiKernel/DataObject.h"
#include "boost/intrusive_ptr.hpp"


inline void intrusive_ptr_add_ref (DataObject* o)
{
  o->addRef();
}


inline void intrusive_ptr_release (DataObject* o)
{
  o->release();
}


namespace SG {


/**
 * @brief Smart pointer to manage @c DataObject reference counts.
 */
template <class T>
class DataObjectSharedPtr
  : public boost::intrusive_ptr<T>
{
public:
  typedef boost::intrusive_ptr<T> Base;
  DataObjectSharedPtr() {}
  explicit DataObjectSharedPtr (T* p, bool add_ref = true)
    : Base (p, add_ref) {}
  DataObjectSharedPtr (const DataObjectSharedPtr& rhs)
    : Base (rhs) {}
  template <class U>
  DataObjectSharedPtr (const DataObjectSharedPtr<U>& rhs)
    : Base (rhs) {}

  // Avoid coverity warning.
  DataObjectSharedPtr& operator= (const DataObjectSharedPtr& rhs) = default;
};


} // namespace SG


#endif // not ATHENAKERNEL_DATAOBJECTSHAREDPTR_H
