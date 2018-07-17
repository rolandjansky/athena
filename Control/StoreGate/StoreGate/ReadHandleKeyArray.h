/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_READHANDLEKEYARRAY_H
#define STOREGATE_READHANDLEKEYARRAY_H 1

#include "StoreGate/HandleKeyArray.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadHandle.h"

namespace SG {

  /**
   * @class SG::ReadHandleKeyArray<T>
   * @brief class to hold an array of ReadHandleKeys
   *
   * since it inherits from std::vector, all vector operations are
   * permitted.
   *
   * initialization can be done in three ways.
   * 1: with an std::vector<ReadHandleKey> as a parameter
   *    SG::ReadHandleKeyArray<foo> m_foo ( std::vector<ReadHandleKey> );
   * 2: with an initializer list of ReadHandleKeys
   *    SG::ReadHandleKeyArray<foo> m_foo { ReadHandleKey<foo> k1, ReadHandleKey<foo> k2 };
   * 3: with an initializer list of std::strings, that will be used to
   *    internally create ReadHandleKeys with those initializers
   *    SG::ReadHandleKeyArray<foo> m_foo { "key1", "key2", "key3" };
   */
  template <class T>
  using ReadHandleKeyArray = HandleKeyArray<ReadHandle<T>, ReadHandleKey<T>, Gaudi::DataHandle::Reader >;


} // namespace SG

#endif
