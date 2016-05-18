/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArROD/LArRawChannelBuilderPedestalSampleZero.h"

LArRawChannelBuilderPedestalSampleZero::LArRawChannelBuilderPedestalSampleZero(const std::string& type,
									       const std::string& name,
									       const IInterface* parent):
  LArRawChannelBuilderPedestalToolBase(type,name,parent)
{
  m_helper = new LArRawChannelBuilderStatistics( 1,      // number of possible errors
					       0x40);  // bit pattern special for this tool,
                                                       // to be stored in  "uint16_t provenance"
  m_helper->setErrorString(0, "no errors");
}

bool LArRawChannelBuilderPedestalSampleZero::pedestal(float& pedestal, MsgStream* /* pLog */ )
{
  pedestal = m_parent->curr_sample0;
  m_helper->incrementErrorCount(0);
  m_parent->qualityBitPattern |= m_helper->returnBitPattern();
  
  return true;
}
