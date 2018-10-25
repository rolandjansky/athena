/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_WRITEDECORHANDLEKEYARRAY_H
#define STOREGATE_WRITEDECORHANDLEKEYARRAY_H 1

#include "StoreGate/DecorHandleKeyArray.h"

#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"

namespace SG {

  /**
   * @class SG::WriteDecorHandleKeyArray<T>
   * @brief class to hold an array of WriteDecorHandleKeys
   *
   * since it inherits from std::vector, all vector operations are
   * permitted.
   *
   * initialization can be done in three ways.
   * 1: with an std::vector<WriteDecorHandleKey> as a parameter
   *    SG::WriteDecorHandleKeyArray<foo> m_foo ( std::vector<WriteDecorHandleKey> );
   * 2: with an initializer list of WriteDecorHandleKeys
   *    SG::WriteDecorHandleKeyArray<foo> m_foo { WriteDecorHandleKey<foo> k1, WriteDecorHandleKey<foo> k2 };
   * 3: with an initializer list of std::strings, that will be used to
   *    internally create WriteDecorHandleKeys with those initializers
   *    SG::WriteDecorHandleKeyArray<foo> m_foo { "key1", "key2", "key3" };
   *
   * Note: makeHandles only works if the type is the same; otherwise have do do individually
   *
   */
  template <class T, class S = float>
  using WriteDecorHandleKeyArray = DecorHandleKeyArray<WriteDecorHandle<T, S>,WriteDecorHandleKey<T>, Gaudi::DataHandle::Writer >;

} // namespace SG

#endif
