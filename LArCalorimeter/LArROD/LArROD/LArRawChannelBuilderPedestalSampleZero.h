/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArRawChannelBuilderPedestalSampleZero
 * @author Rolf Seuster
 * @brief Tool obtaining the Pedestal from the first sample
 */

#ifndef LARROD_LARRAWCHANNELBUILDERPEDESTALSAMPLEZERO_H
#define LARROD_LARRAWCHANNELBUILDERPEDESTALSAMPLEZERO_H

#include "LArROD/LArRawChannelBuilderPedestalToolBase.h"

#include "CLHEP/Units/SystemOfUnits.h"

class LArRawChannelBuilderPedestalSampleZero
: public LArRawChannelBuilderPedestalToolBase
{
 public:
  
  LArRawChannelBuilderPedestalSampleZero(const std::string& type,
					 const std::string& name,
					 const IInterface* parent);
  
  bool pedestal(float& pedestal, MsgStream* pLog);

private:
  LArRawChannelBuilderPedestalSampleZero (const LArRawChannelBuilderPedestalSampleZero&);
  LArRawChannelBuilderPedestalSampleZero& operator= (const LArRawChannelBuilderPedestalSampleZero&);
};

#endif
