/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_READDECORHANDLEKEYARRAY_H
#define STOREGATE_READDECORHANDLEKEYARRAY_H 1

#include "StoreGate/DecorHandleKeyArray.h"

#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/ReadDecorHandle.h"

namespace SG {

  /**
   * @class SG::ReadDecorHandleKeyArray<T>
   * @brief class to hold an array of ReadDecorHandleKeys
   *
   * since it inherits from std::vector, all vector operations are
   * permitted.
   *
   * initialization can be done in three ways.
   * 1: with an std::vector<ReadDecorHandleKey> as a parameter
   *    SG::ReadDecorHandleKeyArray<foo> m_foo ( std::vector<ReadDecorHandleKey> );
   * 2: with an initializer list of ReadDecorHandleKeys
   *    SG::ReadDecorHandleKeyArray<foo> m_foo { ReadDecorHandleKey<foo> k1, ReadDecorHandleKey<foo> k2 };
   * 3: with an initializer list of std::strings, that will be used to
   *    internally create ReadDecorHandleKeys with those initializers
   *    SG::ReadDecorHandleKeyArray<foo> m_foo { "key1", "key2", "key3" };
   *
   * Note: makeHandles only works if the type is the same; otherwise have do do individually
   *
   */
  template <class T, class S = float>
  using ReadDecorHandleKeyArray = DecorHandleKeyArray<ReadDecorHandle<T, S>, ReadDecorHandleKey<T>, Gaudi::DataHandle::Reader >;


} // namespace SG

#endif
