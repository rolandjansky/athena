/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBADCHANNEL_LARBADCHANNELMASK_H
#define LARBADCHANNEL_LARBADCHANNELMASK_H

//#include "CaloIdentifier/CaloGain.h"           
#include "Identifier/HWIdentifier.h" 
#include "LArRecConditions/LArBadChannelEnum.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include <vector>
#include <string>


class LArBadChannelMask {
 public:
  StatusCode buildBitMask(const std::vector<std::string>& problemsToMask, MsgStream& msg);


  bool cellShouldBeMasked(const LArBadChannelCont* bcCont, const HWIdentifier& hardwareId) const;
  bool cellShouldBeMasked(const LArBadChannelCont* bcCont, const Identifier& offlineId) const;

 private:
  const static LArBadChanBitPacking  s_bitPacking; // A helper for bit operations, etc.
  typedef LArBadChannelEnum::BitWord BitWord;
  BitWord  m_bitMask=0x0;  // The list of problems in bit form.
};



inline 
bool LArBadChannelMask::cellShouldBeMasked(const LArBadChannelCont* bcCont, 
					   const HWIdentifier& hardwareId) const {

  const LArBadChannel cellStatus=bcCont->status(hardwareId);
  return (m_bitMask & cellStatus.packedData()) != 0;
}

inline 
bool LArBadChannelMask::cellShouldBeMasked(const LArBadChannelCont* bcCont, 
					   const Identifier& offlineId) const {

  const LArBadChannel cellStatus=bcCont->offlineStatus(offlineId);
  return (m_bitMask & cellStatus.packedData()) != 0;
}

#endif
