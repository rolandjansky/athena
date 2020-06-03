/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloUtils/DataError.h"

namespace LVL1 {

const int DataError::s_failingBcnMask;
const int DataError::s_ppmErrorMask;
const int DataError::s_subStatusMask;
const int DataError::s_subStatusBcn;

DataError::DataError() : m_error(0)
{
}

DataError::DataError(int error) : m_error(error)
{
}

DataError::~DataError()
{
}

// Set an error bit or data

void DataError::set(ErrorBit bit, int value)
{
  if (bit == SubStatusWord) {
    m_error &= ~(s_subStatusMask << GLinkParity);
    m_error |= (value & s_subStatusMask) << GLinkParity;
    bit = FailingBCN;
    value >>= s_subStatusBcn;
  }
  int mask = 0x1;
  if (bit == FailingBCN) mask = s_failingBcnMask;
  else if (bit == PPMErrorWord) {
    bit  = ChannelDisabled;
    mask = s_ppmErrorMask;
  }
  m_error &= ~(mask << bit);
  m_error |= (value & mask) << bit;
}

// Return an error bit or data

int DataError::get(ErrorBit bit) const
{
  if (bit == SubStatusWord) return 0;
  int mask = 0x1;
  if (bit == FailingBCN) mask = s_failingBcnMask;
  else if (bit == PPMErrorWord) {
    bit  = ChannelDisabled;
    mask = s_ppmErrorMask;
  }
  return (m_error >> bit) & mask;
}

// Return an error bit or data

int DataError::get(int bit) const
{
  int mask = (bit == FailingBCN) ? s_failingBcnMask : 0x1;
  return (m_error >> bit) & mask;
}

// Return the name of an error bit

std::string DataError::bitName(int bit) const
{
  std::string name;
  switch (bit) {
    case Overflow:        name = "Overflow";        break;
    case Parity:          name = "Parity";          break;
    case LinkDown:        name = "LinkDown";        break;
    case ChannelDisabled: name = "ChannelDisabled"; break;
    case MCMAbsent:       name = "MCMAbsent";       break;
    case Timeout:         name = "Timeout";         break;
    case ASICFull:        name = "ASICFull";        break;
    case EventMismatch:   name = "EventMismatch";   break;
    case BunchMismatch:   name = "BunchMismatch";   break;
    case FIFOCorrupt:     name = "FIFOCorrupt";     break;
    case PinParity:       name = "PinParity";       break;
    //case ParityMerge:     name = "ParityMerge";     break;
    //case ParityPhase0:    name = "ParityPhase0";    break;
    //case ParityPhase1:    name = "ParityPhase1";    break;
    //case ParityPhase2:    name = "ParityPhase2";    break;
    //case ParityPhase3:    name = "ParityPhase3";    break;
    case GLinkParity:     name = "GLinkParity";     break;
    case GLinkProtocol:   name = "GLinkProtocol";   break;
    case BCNMismatch:     name = "BCNMismatch";     break;
    case FIFOOverflow:    name = "FIFOOverflow";    break;
    case ModuleError:     name = "ModuleError";     break;
    case GLinkDown:       name = "GLinkDown";       break;
    case GLinkTimeout:    name = "GLinkTimeout";    break;
    default:              name = "";                break;
  }
  return name;
}

} // end namespace
