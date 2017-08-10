#include "LArRecConditions/LArOnOffIdMapping.h"

LArOnOffIdMapping::LArOnOffIdMapping(const LArOnlineID_Base* onlineId, const CaloCell_Base_ID * caloId) :

  m_onlineId(onlineId),
  m_caloId(caloId)

{
  unsigned nChan=m_onlineId->channelHashMax();
  m_onlHashToOffline.assign(nChan,m_idEmpty);
  m_oflHashToOnline.assign(nChan,m_hwidEmpty);

}


