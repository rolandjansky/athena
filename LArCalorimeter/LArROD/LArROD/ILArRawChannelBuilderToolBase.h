/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class ILArRawChannelBuilderToolBase
 * @author Rolf Seuster
 * @brief Interface definition for tools to build LArRawChannels
 * 
 */

#ifndef LARROD_ILARRAWCHANNELBUILDERTOOLBASE_H
#define LARROD_ILARRAWCHANNELBUILDERTOOLBASE_H

#include "LArROD/ILArRawChannelBuilderToolBaseClass.h"

class LArDigit;

// Declaration of the interfac ID
static const InterfaceID IID_ILArRawChannelBuilderToolBase ("ILArRawChannelBuiderToolBase", 1, 0 );

class ILArRawChannelBuilderToolBase
: public virtual ILArRawChannelBuilderToolBaseClass
{
 public:
  
  ~ILArRawChannelBuilderToolBase() { };
  
  // retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ILArRawChannelBuilderToolBase; }
  
  virtual bool buildRawChannel(const LArDigit* digit,
			       float pedestal,
			       const std::vector<float>& ramps,
			       MsgStream* pLog) = 0;
  virtual int energy() = 0;
  virtual int time() = 0;
  virtual CaloGain::CaloGain gain() = 0;
  
};
#endif

