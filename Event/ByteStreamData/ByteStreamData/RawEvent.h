/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#ifndef BYTESTREAMCNVSVC_RAWEVENT_H
#define BYTESTREAMCNVSVC_RAWEVENT_H

//
//  Initial version:  July 10, 2002
//        typedef for RawEvent from eformat.
//---------------------------------------------------------------------

#include <inttypes.h>

#include "eformat/FullEventFragment.h"
#include "eformat/ROBFragment.h"
#include "eformat/write/FullEventFragment.h"
#include "eformat/write/ROBFragment.h"

namespace OFFLINE_FRAGMENTS_NAMESPACE {
  /*@name typedefs to read fragments */
  typedef uint32_t DataType;
  typedef const DataType * PointerType;
  typedef eformat::FullEventFragment<PointerType>   FullEventFragment;
  typedef eformat::ROBFragment<PointerType>         ROBFragment;
}

namespace OFFLINE_FRAGMENTS_NAMESPACE_WRITE {
  /*@name typedefs to write fragments */
  typedef eformat::write::FullEventFragment   FullEventFragment;
  typedef eformat::write::ROBFragment         ROBFragment;
}

/// data type for reading raw event
typedef OFFLINE_FRAGMENTS_NAMESPACE::FullEventFragment       RawEvent;
/// data type for writing raw event
typedef OFFLINE_FRAGMENTS_NAMESPACE_WRITE::FullEventFragment RawEventWrite;

#endif

#endif
