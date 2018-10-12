/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_WRITEHANDLEKEYARRAY_H
#define STOREGATE_WRITEHANDLEKEYARRAY_H 1

#include "StoreGate/HandleKeyArray.h"

#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteHandle.h"

namespace SG {

  /**
   * @class SG::WriteHandleKeyArray<T>
   * @brief class to hold an array of WriteHandleKeys
   *
   * since it inherits from std::vector, all vector operations are
   * permitted.
   *
   * initialization can be done in three ways.
   * 1: with an std::vector<WriteHandleKey> as a parameter
   *    SG::WriteHandleKeyArray<foo> m_foo ( std::vector<WriteHandleKey> );
   * 2: with an initializer list of WriteHandleKeys
   *    SG::WriteHandleKeyArray<foo> m_foo { WriteHandleKey<foo> k1, WriteHandleKey<foo> k2 };
   * 3: with an initializer list of std::strings, that will be used to
   *    internally create WriteHandleKeys with those initializers
   *    SG::WriteHandleKeyArray<foo> m_foo { "key1", "key2", "key3" };
   */
  template <class T>
  using WriteHandleKeyArray = HandleKeyArray<WriteHandle<T>,WriteHandleKey<T>, Gaudi::DataHandle::Writer >;

} // namespace SG

#endif
