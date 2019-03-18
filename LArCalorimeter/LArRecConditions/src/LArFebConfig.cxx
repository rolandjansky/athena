/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecConditions/LArFebConfig.h"
#include "LArIdentifier/LArOnlineID.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "CoralBase/Attribute.h"

LArFebConfig::LArFebConfig(const LArOnlineID* onlineId) : 
  AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc" ),"LArFebConfig"),
  m_onlineID(onlineId)

{ }

short LArFebConfig::getThreshold(const char* MedLow, const HWIdentifier& chid) const {

  if (m_attrPerFeb.size()==0) {
    ATH_MSG_WARNING("FEB treshold cache is empty. Callback not fired?");
    return ERRORCODE;
  }

  std::lock_guard<std::mutex> lock(m_itMtx); //Makes sure the following isn't executed concurently

  const HWIdentifier fid=m_onlineID->feb_Id(chid);
  const int channel=m_onlineID->channel(chid); 
  if (m_lastIt==m_attrPerFeb.end() || m_lastIt->first!=fid)
    m_lastIt=m_attrPerFeb.find(fid);

  if (m_lastIt==m_attrPerFeb.end()) { 
    ATH_MSG_DEBUG("Such FEB was not found !");
    return ERRORCODE;
  }else {
     std::string channame(MedLow,5);
     channame += std::to_string(channel+1);
    return (short)(*(m_lastIt->second))[channame].data<int32_t>(); //Will throw and exception if channel does not exist
  } 
}
