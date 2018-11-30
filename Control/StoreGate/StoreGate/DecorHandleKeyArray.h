/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_DECORHANDLEKEYARRAY_H
#define STOREGATE_DECORHANDLEKEYARRAY_H 1

#include "StoreGate/HandleKeyArray.h"

namespace SG {

  /**
   * @class SG::DecorHandleKeyArray<T>
   * @brief class to hold an array of DecorHandleKeys
   *
   * since it inherits from std::vector, all vector operations are
   * permitted.
   *
   * This can be converted to a class if it needs customization
   *
   * Note: makeHandles only works if the type is the same; otherwise have do do individually
   *
   */
  template <class T_Handle, class T_HandleKey, Gaudi::DataHandle::Mode MODE>
  using DecorHandleKeyArray = HandleKeyArray< T_Handle, T_HandleKey, MODE>;

} // namespace SG

#endif
