/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArRawChannelBuilderADC2EToolBase
 * @author Rolf Seuster
 * @brief Interface definition for Tools to Convert ADC to energy
 */

#ifndef LARROD_ILARRAWCHANNELBUILDERADC2ETOOLBASE_H
#define LARROD_ILARRAWCHANNELBUILDERADC2ETOOLBASE_H

#include "LArROD/ILArRawChannelBuilderToolBaseClass.h"

#include <vector>

// Declaration of the interfac ID
static const InterfaceID IID_ILArRawChannelBuilderADC2EToolBase ("ILArRawChannelBuiderADC2EToolBase", 1, 0 );

class ILArRawChannelBuilderADC2EToolBase
: public virtual ILArRawChannelBuilderToolBaseClass
{
 public:
  
  ~ILArRawChannelBuilderADC2EToolBase() { };
  
  // retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ILArRawChannelBuilderADC2EToolBase; }
  
  virtual bool ADC2E(std::vector<float>& Ramps, MsgStream* pLog) =0;
  
};
#endif

