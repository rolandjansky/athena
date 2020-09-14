/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ASG_DATA_HANDLES_WRITE_DECOR_HANDLE_KEY_ARRAY_H
#define ASG_DATA_HANDLES_WRITE_DECOR_HANDLE_KEY_ARRAY_H

#ifndef XAOD_STANDALONE
#include <StoreGate/WriteDecorHandleKeyArray.h>
#else

#include "AsgDataHandles/DecorHandleKeyArray.h"

#include "AsgDataHandles/WriteDecorHandleKey.h"
#include "AsgDataHandles/WriteDecorHandle.h"

namespace SG {

  /**
   * @class SG::WriteDecorHandleKeyArray<T>
   * @brief class to hold an array of WriteDecorHandleKeys
   *
   * See StoreGate/WriteDecorHandleKeyArray for details.
   *
   */
  template <class T, class S = float>
  using WriteDecorHandleKeyArray = DecorHandleKeyArray<WriteDecorHandle<T, S>,WriteDecorHandleKey<T>/*, Gaudi::DataHandle::Writer*/ >;

} // namespace SG

#endif

#endif
