/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_CONDHANDLEKEYARRAY_H
#define STOREGATE_CONDHANDLEKEYARRAY_H 1

#include "StoreGate/HandleKeyArray.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/WriteCondHandle.h"

namespace SG {

  namespace CondHandleDefault {
    // need default constructor for VarHandleKeyArrayCommon<Base>::assign
    template <class Base>
    class Key : public Base {
    public:
      Key()  : Base("") {}

      Key(const std::string& val)  : Base(val) {}

    };
  }

  template <class T>
  using ReadCondHandleKeyArray  = HandleKeyArray<ReadCondHandle<T>, CondHandleDefault::Key<ReadCondHandleKey<T> >,  Gaudi::DataHandle::Reader >;

  template <class T>
  using WriteCondHandleKeyArray = HandleKeyArray<WriteCondHandle<T>,CondHandleDefault::Key<WriteCondHandleKey<T> >, Gaudi::DataHandle::Writer >;

} // namespace SG

#endif
