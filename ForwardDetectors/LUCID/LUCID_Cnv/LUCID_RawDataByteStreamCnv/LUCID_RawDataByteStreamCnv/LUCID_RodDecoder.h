/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_RODDECODER_H
#define LUCID_RODDECODER_H

#include <inttypes.h>

#include "ByteStreamData/RawEvent.h"
#include "GaudiKernel/MsgStream.h"

class LUCID_RodDecoder {
  
 public: 
  StatusCode decode(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment* robFragment,
                    std::vector<uint32_t>& data_block,
                    MsgStream& msg) const;
  
  unsigned int getSourceID() const { return 0x00820000; }
}; 

#endif
