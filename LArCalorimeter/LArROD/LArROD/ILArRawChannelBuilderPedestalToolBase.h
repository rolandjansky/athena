/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArRawChannelBuilderPedestalToolBase
 * @author Rolf Seuster
 * @brief extention of Interface definition for Tools for Pedestal Obtention
 */


#ifndef LARROD_ILARRAWCHANNELBUILDERPEDESTALTOOLBASE_H
#define LARROD_ILARRAWCHANNELBUILDERPEDESTALTOOLBASE_H

#include "LArROD/ILArRawChannelBuilderToolBaseClass.h"

// Declaration of the interfac ID
static const InterfaceID IID_ILArRawChannelBuilderPedestalToolBase ("ILArRawChannelBuiderPedestalToolBase", 1, 0 );

class ILArRawChannelBuilderPedestalToolBase
: public virtual ILArRawChannelBuilderToolBaseClass
{
 public:
  
  ~ILArRawChannelBuilderPedestalToolBase() { };
  
  // retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ILArRawChannelBuilderPedestalToolBase; }
  
  virtual bool pedestal(float& pedestal, MsgStream* pLog) =0;
  
};

#endif
