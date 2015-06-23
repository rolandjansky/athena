/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArROD/LArRawChannelBuilderPedestalDataBase.h"

LArRawChannelBuilderPedestalDataBase::LArRawChannelBuilderPedestalDataBase(const std::string& type,
									   const std::string& name,
									   const IInterface* parent):
  LArRawChannelBuilderPedestalToolBase(type,name,parent)
{
  helper = new LArRawChannelBuilderStatistics( 2,      // number of possible errors
					       0x80);  // bit pattern special for this tool,
                                                       // to be stored in  "uint16_t provenance"
  helper->setErrorString(0, "no errors");
  helper->setErrorString(1, "no DataBase");
  helper->setErrorString(2, "no DataBase entry");
  
  declareProperty("LArPedestalKey",    m_pedestalKey="LArPedestal");
}

StatusCode LArRawChannelBuilderPedestalDataBase::initTool()
{
  if (detStore()->regHandle(m_larPedestal,m_pedestalKey).isFailure()){
    msg(MSG::ERROR) << "No pedestals with key <" << m_pedestalKey << "> found in database." << endreq;
    helper->incrementErrorCount(1);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

bool LArRawChannelBuilderPedestalDataBase::pedestal(float& pedestal, MsgStream* pLog)
{
  pedestal = 0;
  
  // use database for pedestals
  if(!bool(m_larPedestal))
    {
      if(bool(pLog))
	(*pLog) << MSG::WARNING << "No pedestal found in database."
	     << endreq;
      helper->incrementErrorCount(1);
      return false;
    }
  
  float DBpedestal 
    =m_larPedestal->pedestal(pParent->curr_chid,pParent->curr_gain);
  if (DBpedestal <= (1.0+LArElecCalib::ERRORCODE))
    {
      helper->incrementErrorCount(2);
      return false;
    }
  pedestal=DBpedestal;
  helper->incrementErrorCount(0);
  pParent->qualityBitPattern |= helper->returnBitPattern();
  
  return true;
}
