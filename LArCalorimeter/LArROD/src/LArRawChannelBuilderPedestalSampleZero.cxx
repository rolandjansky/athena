/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArROD/LArRawChannelBuilderPedestalSampleZero.h"

LArRawChannelBuilderPedestalSampleZero::LArRawChannelBuilderPedestalSampleZero(const std::string& type,
									       const std::string& name,
									       const IInterface* parent):
  LArRawChannelBuilderPedestalToolBase(type,name,parent)
{
  helper = new LArRawChannelBuilderStatistics( 1,      // number of possible errors
					       0x40);  // bit pattern special for this tool,
                                                       // to be stored in  "uint16_t provenance"
  helper->setErrorString(0, "no errors");
}

bool LArRawChannelBuilderPedestalSampleZero::pedestal(float& pedestal, MsgStream* /* pLog */ )
{
  pedestal = pParent->curr_sample0;
  helper->incrementErrorCount(0);
  pParent->qualityBitPattern |= helper->returnBitPattern();
  
  return true;
}
