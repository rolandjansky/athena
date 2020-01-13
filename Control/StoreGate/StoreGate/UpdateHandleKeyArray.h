/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_UPDATEHANDLEKEYARRAY_H
#define STOREGATE_UPDATEHANDLEKEYARRAY_H 1

#include "StoreGate/HandleKeyArray.h"

#include "StoreGate/UpdateHandleKey.h"
#include "StoreGate/UpdateHandle.h"

namespace SG {

  /**
   * @class SG::UpdateHandleKeyArray<T>
   * @brief class to hold an array of UpdateHandleKeys
   *
   * since it inherits from std::vector, all vector operations are
   * permitted.
   *
   * initialization can be done in three ways.
   * 1: with an std::vector<UpdateHandleKey> as a parameter
   *    SG::UpdateHandleKeyArray<foo> m_foo ( std::vector<UpdateHandleKey> );
   * 2: with an initializer list of UpdateHandleKeys
   *    SG::UpdateHandleKeyArray<foo> m_foo { UpdateHandleKey<foo> k1, UpdateHandleKey<foo> k2 };
   * 3: with an initializer list of std::strings, that will be used to
   *    internally create UpdateHandleKeys with those initializers
   *    SG::UpdateHandleKeyArray<foo> m_foo { "key1", "key2", "key3" };
   */
  template <class T>
  using UpdateHandleKeyArray = HandleKeyArray<UpdateHandle<T>,UpdateHandleKey<T>, Gaudi::DataHandle::Reader >;

} // namespace SG

#endif
