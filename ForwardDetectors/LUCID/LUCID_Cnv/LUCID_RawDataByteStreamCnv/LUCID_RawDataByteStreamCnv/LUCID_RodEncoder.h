/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_RODENCODER_H
#define LUCID_RODENCODER_H

#include <inttypes.h>

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/MsgStreamMember.h"

#include "ByteStreamData/RawEvent.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "LUCID_RawEvent/LUCID_RawData.h"
#include "LUCID_RawEvent/LUCID_Digit.h"

class LUCID_RodEncoder {
  
 public: 
  
  typedef std::vector<const LUCID_Digit*> VDIGIT;
  
  LUCID_RodEncoder();
  ~LUCID_RodEncoder();

  void addDigit(const LUCID_Digit* digit) { m_Digits.push_back(digit); }
  void encode(std::vector<uint32_t>& data_block);
  
  MsgStream& msg(MSG::Level lvl) const { return m_msg << lvl; }
  
  bool msgLevel(MSG::Level lvl){ return m_msg.get().level() <= lvl; }

  VDIGIT getDigits() { return m_Digits;}
  
 private:

  unsigned int m_hitcounter0;
  unsigned int m_hitcounter1;
  unsigned int m_hitcounter2;
  unsigned int m_hitcounter3;

  VDIGIT m_Digits;
  
  mutable Athena::MsgStreamMember m_msg;
}; 

#endif
