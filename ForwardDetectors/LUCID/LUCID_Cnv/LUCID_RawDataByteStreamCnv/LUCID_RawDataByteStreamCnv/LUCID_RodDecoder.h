/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_RODDECODER_H
#define LUCID_RODDECODER_H

#include <inttypes.h>

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/MsgStreamMember.h"

#include "ByteStreamData/RawEvent.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

class LUCID_RodDecoder {
  
 public: 
  
  LUCID_RodDecoder();
  ~LUCID_RodDecoder();
  
  StatusCode decode(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment* robFragment, std::vector<uint32_t>& data_block);
  
  MsgStream& msg(MSG::Level lvl) const { return m_msg << lvl; }
  
  bool msgLevel(MSG::Level lvl){ return m_msg.get().level() <= lvl; }

  unsigned int getSourceID() { return 0x00820000; }
  
 private:
  
  mutable Athena::MsgStreamMember m_msg;
}; 

#endif
