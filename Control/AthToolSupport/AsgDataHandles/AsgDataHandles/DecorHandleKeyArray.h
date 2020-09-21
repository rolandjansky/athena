/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASG_DATA_HANDLES_DECOR_HANDLE_KEY_ARRAY_H
#define ASG_DATA_HANDLES_DECOR_HANDLE_KEY_ARRAY_H


#ifndef XAOD_STANDALONE
#include <StoreGate/DecorHandleKeyArray.h>
#else

#include <AsgDataHandles/HandleKeyArray.h>

namespace SG {

  /**
   * @class SG::DecorHandleKeyArray<T>
   * @brief class to hold an array of DecorHandleKeys
   *
   * See StoreGate/DecorHandleKeyArray for details.
   *
   */
  template <class T_Handle, class T_HandleKey/*, Gaudi::DataHandle::Mode MODE*/>
  using DecorHandleKeyArray = HandleKeyArray< T_Handle, T_HandleKey/*, MODE*/>;

} // namespace SG

#endif

#endif
