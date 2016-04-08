/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArROD/LArRawChannelBuilderToolBadChannelTool.h"
#include "GaudiKernel/MsgStream.h"

#include "Identifier/Identifier.h"

LArRawChannelBuilderToolBadChannelTool::LArRawChannelBuilderToolBadChannelTool(const std::string& type,
									       const std::string& name,
									       const IInterface* parent):
  LArRawChannelBuilderToolBase(type,name,parent),
  m_badChannelMask("BadLArRawChannelMask",this)
{
  m_helper = new LArRawChannelBuilderStatistics( 2,      // number of possible errors
					       0x01);  // bit pattern special for this tool,
                                                       // to be stored in "int quality"
  m_helper->setErrorString(0, "no errors");
  m_helper->setErrorString(1, "known bad channel");
  declareProperty("BadChannelMask",m_badChannelMask);

}

StatusCode LArRawChannelBuilderToolBadChannelTool::initTool()
{
  MsgStream log(msgSvc(), name());
  if(m_badChannelMask.retrieve().isFailure())
    {
      log << MSG::ERROR << "Could not retrieve private BadChannelMask "
	  << m_badChannelMask << endreq;
      return StatusCode::FAILURE;
    }
  
  return StatusCode::SUCCESS;
}

bool LArRawChannelBuilderToolBadChannelTool::buildRawChannel(const LArDigit* /*digit*/,
							     float /*pedestal*/,
							     const std::vector<float>& /*ramps*/,
							     MsgStream* /* pLog */ )
{
  // zero means channel ok
  const HWIdentifier chid=m_parent->curr_chid;
  const CaloGain::CaloGain gain=m_parent->curr_gain;
  if (m_badChannelMask->cellShouldBeMasked(chid,gain)) {
    // inverse logic, true means building went ok.
    m_helper->incrementErrorCount(1);
    return true;
  }
  
  m_helper->incrementErrorCount(0);
  return false;
}

