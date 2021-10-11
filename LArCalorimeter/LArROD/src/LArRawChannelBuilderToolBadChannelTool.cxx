/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArROD/LArRawChannelBuilderToolBadChannelTool.h"
#include "GaudiKernel/MsgStream.h"

#include "Identifier/Identifier.h"

LArRawChannelBuilderToolBadChannelTool::LArRawChannelBuilderToolBadChannelTool(const std::string& type,
									       const std::string& name,
									       const IInterface* parent):
  LArRawChannelBuilderToolBase(type,name,parent)
{
  m_helper = new LArRawChannelBuilderStatistics( 2,      // number of possible errors
					       0x01);  // bit pattern special for this tool,
                                                       // to be stored in "int quality"
  m_helper->setErrorString(0, "no errors");
  m_helper->setErrorString(1, "known bad channel");

}

StatusCode LArRawChannelBuilderToolBadChannelTool::initTool()
{
  MsgStream log(msgSvc(), name());
  ATH_CHECK(m_bcContKey.initialize());
  ATH_CHECK(m_bcMask.buildBitMask(m_problemsToMask,log));
  
  return StatusCode::SUCCESS;
}

bool LArRawChannelBuilderToolBadChannelTool::buildRawChannel(const LArDigit* /*digit*/,
							     float /*pedestal*/,
							     const std::vector<float>& /*ramps*/,
							     MsgStream* /* pLog */ )
{
  //retrieve BadChannel info:
  const LArBadChannelCont* bcCont=nullptr;
  SG::ReadCondHandle<LArBadChannelCont> bcContHdl{m_bcContKey};
  bcCont=(*bcContHdl);
  
  // zero means channel ok
  const HWIdentifier chid=m_parent->curr_chid;
  if (m_bcMask.cellShouldBeMasked(bcCont,chid)) {
    // inverse logic, true means building went ok.
    m_helper->incrementErrorCount(1);
    return true;
  }
  
  m_helper->incrementErrorCount(0);
  return false;
}

