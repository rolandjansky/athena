/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecConditions/LArFebConfig.h"
#include "LArIdentifier/LArOnlineID.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "CoralBase/Attribute.h"

const std::string LArFebConfig::s_lower = "lower";
const std::string LArFebConfig::s_upper = "upper";

LArFebConfig::LArFebConfig(const LArOnlineID* onlineId) : 
  AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc" ),"LArFebConfig"),
  m_onlineID(onlineId)

{ }


void LArFebConfig::add (HWIdentifier febid,
                        const coral::AttributeList* attrList)
{
  m_attrPerFeb[febid.get_compact()] = attrList;
}


void LArFebConfig::thresholds (const HWIdentifier& chid,
                               short& lower,
                               short& upper) const
{
  int channel = 0;
  const coral::AttributeList* attrList = getAttrList (chid, channel);
  if (attrList == nullptr) {
    lower = ERRORCODE;
    upper = ERRORCODE;
    return;
  }
  std::string chanstr = std::to_string(channel+1);
  lower =  getThresholdFromAttrList (s_lower, attrList, chanstr);
  upper =  getThresholdFromAttrList (s_upper, attrList, chanstr);
}


const coral::AttributeList*
LArFebConfig::getAttrList (const HWIdentifier& chid,
                           int& channel) const
{
  if (m_attrPerFeb.empty()) {
    ATH_MSG_WARNING("FEB threshold cache is empty");
    return nullptr;
  }

  const HWIdentifier fid=m_onlineID->feb_Id(chid);
  channel = m_onlineID->channel(chid); 
  auto it = m_attrPerFeb.find(fid.get_compact());
  if (it == m_attrPerFeb.end()) { 
    ATH_MSG_DEBUG("Such FEB was not found !");
    return nullptr;
  }
  return it->second;
}


short
LArFebConfig::getThresholdFromAttrList(const std::string& MedLow,
                                       const coral::AttributeList* attrList,
                                       const std::string& chanstr) const
{
  std::string channame = MedLow + chanstr;
  return (short)(*attrList)[channame].data<int32_t>(); //Will throw and exception if channel does not exist
}


short LArFebConfig::getThreshold(const std::string& MedLow,
                                 const HWIdentifier& chid) const
{
  int channel = 0;
  const coral::AttributeList* attrList = getAttrList (chid, channel);
  if (attrList == nullptr) {
    return ERRORCODE;
  }
  std::string chanstr = std::to_string(channel+1);
  return getThresholdFromAttrList (MedLow, attrList, chanstr);
}
