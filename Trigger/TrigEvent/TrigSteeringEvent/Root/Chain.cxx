/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : Chain
 *
 * @brief  chain of signatures
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: Chain.cxx,v 1.59 2009-02-16 15:47:05 tbold Exp $
 **********************************************************************************/

#include "TrigConfHLTData/HLTSignature.h"
#include "TrigSteeringEvent/Chain.h"


#include <sstream>
#include <iostream>

using namespace HLT;
Chain::Chain( uint32_t serialData )
  : m_configChain(0) {
  reset();
  deserialize(serialData);
  setStatus(HLT::ChainInvalid);
} 

Chain::Chain(const TrigConf::HLTChain* configChain) 
  : m_configChain(configChain) 
  , m_chain_counter{0}
{
  if (configChain)
    m_chain_counter = m_configChain->chain_counter();

  reset();
  setStatus(HLT::ConfigOnlyChain);
}


bool Chain::chainPassed() const { 
  return (chainPassedRaw() && (!isPrescaled()) && (!isResurrected())) || isPassedThrough(); 
} 



HLT::ErrorCode Chain::serialize( std::vector<uint32_t>& output ) const
{
  // Chain serialized structure: 1 word per Chain
  // 1 bit: active ?  (before prescale and passThrough)
  // 1 bit: prescaled ?
  // 1 bit: passedThrough ?
  // 7 bit: last active step => [0,127]
  // 16 bit: chainCounter    => [0,65535]
  // 6 bit: ErrorCode        => [0,63]

  // serialize data: first 3 bits for Active, Prescaled, passThrough bits
  uint32_t chainWord = 0;
  if (chainPassedRaw())  chainWord |= 0x1;
  if (isPrescaled())     chainWord |= 0x2;
  if (isPassedThrough()) chainWord |= 0x4;

  // next, 6 bits for the last active step (64 steps)
  chainWord |= ((m_currentStep & 0x0000003f) << 3);

  
  if ( isResurrected()) chainWord |= 0x200;
  
  // next, 16 bits for the chain counter
  chainWord |= ((getChainCounter() & 0x0000ffff) << (3+6+1));

  // next, 6 bits for the errorCode
  chainWord |= ((m_errorCode & 0x0000003f) << (3+6+1+16));

  output.push_back(chainWord);

  return HLT::OK;
}

HLT::ErrorCode Chain::setDecisions(bool passedraw, bool passedthrough, bool prescaled, bool resurrected)
{
  m_status = ChainOK;
  m_passedRaw = passedraw;
  m_passThrough    = passedthrough;
  m_prescaled      = prescaled;
  m_resurrected    = resurrected;

  // can't deduce the following
  // m_currentStep    = 0; // can't deduce
  // m_chain_counter  = ( chainWord >> (3+7)) & 0x0000ffff;
  // m_errorCode      = static_cast<HLT::ErrorCode>(( chainWord >> (3+7+16)) & 0x0000003f);
  return HLT::OK;
}


HLT::ErrorCode Chain::deserialize(uint32_t chainWord)
{
  m_status = ChainOK;
  m_passedRaw = chainWord & (0x1);
  m_passThrough    = chainWord & (0x4);
  m_prescaled      = chainWord & (0x2);
  m_resurrected    = chainWord & (0x200);
  
  m_currentStep    = ( chainWord >> 3)   & 0x0000003f;
  m_chain_counter  = ( chainWord >> (3+7)) & 0x0000ffff;
  m_errorCode      = static_cast<HLT::ErrorCode>(( chainWord >> (3+7+16)) & 0x0000003f);
  /*
  // Put m_currentStep signatures in the vector
  for (unsigned int i = 0; i < static_cast<unsigned int>(m_currentStep); i++) m_signatures.push_back(0);
  
  // if chain didn't pass, add one more signature...
  if (!(chainWord & (0x1)))  m_signatures.push_back(0);
  */

  return HLT::OK;
}

unsigned int HLT::Chain::inquireChainCounter(uint32_t chainWord) {
  return ( chainWord >> (3+7)) & 0x0000ffff;
}

bool HLT::Chain::reset() {
  m_errorCode   = HLT::OK;
  m_passedRaw   = false;
  m_passThrough = false;
  m_prescaled   = false;
  m_resurrected = false;
  m_currentStep = 0;
  return true;
}


/** Overload of << operator for both, MsgStream and std::ostream */
std::ostream& HLT::operator << (std::ostream& os, const Chain& c)
{
  c.print(os);
  return os;
}
MsgStream& HLT::operator << (MsgStream& msg, const Chain& c)
{
  c.print(msg);
  return msg;
}

