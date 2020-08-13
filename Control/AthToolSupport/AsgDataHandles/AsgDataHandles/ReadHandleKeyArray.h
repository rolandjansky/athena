/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASG_DATA_HANDLES_READ_HANDLE_KEY_ARRAY_H
#define ASG_DATA_HANDLES_READ_HANDLE_KEY_ARRAY_H

#ifndef XAOD_STANDALONE
#include "StoreGate/ReadHandleKeyArray.h"
#else

#include "AsgDataHandles/HandleKeyArray.h"

#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgDataHandles/ReadHandle.h"

namespace SG {

  /**
   * @class SG::ReadHandleKeyArray<T>
   * @brief class to hold an array of ReadHandleKeys
   *
   * See StoreGate/ReadHandleKeyArray for details.
   *
   * This currently (13 Aug 20) contains a commented out template
   * argument that was part of the original handle implementation, but
   * has not yet been implemented in the standalone version.  The plan
   * is to either implement or remove them, depending on what is
   * needed in AnalysisBase as we add more packages.
   */
  template <class T>
  using ReadHandleKeyArray = HandleKeyArray<ReadHandle<T>, ReadHandleKey<T>/*, Gaudi::DataHandle::Reader*/ >;


} // namespace SG

#endif

#endif
