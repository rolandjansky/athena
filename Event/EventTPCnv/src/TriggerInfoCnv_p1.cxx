/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfo/TriggerInfo.h"
#include "EventTPCnv/TriggerInfoCnv_p1.h"
#include "CxxUtils/make_unique.h"


void TriggerInfoCnv_p1::transToPers(const TriggerInfo* trans, TriggerInfo_p1* pers, MsgStream &log)
{
  const TriggerInfoCnv_p1* cthis = this;
  cthis->transToPers (trans, pers, log);
}

void TriggerInfoCnv_p1::transToPers(const TriggerInfo* trans, TriggerInfo_p1* pers, MsgStream &) const
{
    // Deprecated, p2 is used for writing
     pers->m_extendedLevel1ID    = trans->extendedLevel1ID();
     pers->m_level1TriggerType   = trans->level1TriggerType();
     pers->m_level2TriggerInfo   = 0;
     if (!trans->level2TriggerInfo().empty())
       pers->m_level2TriggerInfo   = trans->level2TriggerInfo()[0];
     pers->m_eventFilterInfo     = trans->eventFilterInfo();
}


void TriggerInfoCnv_p1::persToTrans(const TriggerInfo_p1* pers, TriggerInfo* trans, MsgStream &log)
{
  const TriggerInfoCnv_p1* cthis = this;
  cthis->persToTrans (pers, trans, log);
}

void TriggerInfoCnv_p1::persToTrans(const TriggerInfo_p1* pers, TriggerInfo* trans, MsgStream &)  const
{
    trans->setExtendedLevel1ID    (pers->m_extendedLevel1ID);
    trans->setLevel1TriggerType   (pers->m_level1TriggerType);
    // For backwards compatibility - add pers->m_level2TriggerInfo to
    // trans vector if not zero
    if (pers->m_level2TriggerInfo != 0) {
      std::vector<TriggerInfo::number_type> l2info {pers->m_level2TriggerInfo};
      trans->setLevel2TriggerInfo (std::move (l2info));
    }
    trans->setEventFilterInfo      (pers->m_eventFilterInfo);
}


TriggerInfo* TriggerInfoCnv_p1::createTransient (const TriggerInfo_p1* persObj, MsgStream& log)
{
  const TriggerInfoCnv_p1* cthis = this;
  return cthis->createTransient (persObj, log);
}

TriggerInfo* TriggerInfoCnv_p1::createTransient (const TriggerInfo_p1* persObj, MsgStream& log) const
{
  auto trans = CxxUtils::make_unique<TriggerInfo>();
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}

TriggerInfo_p1* TriggerInfoCnv_p1::createPersistent (const TriggerInfo* transObj, MsgStream& log)
{
  const TriggerInfoCnv_p1* cthis = this;
  return cthis->createPersistent (transObj, log);
}

TriggerInfo_p1* TriggerInfoCnv_p1::createPersistent (const TriggerInfo* transObj, MsgStream& log) const
{
  auto pers = CxxUtils::make_unique<TriggerInfo_p1>();
  transToPers(transObj, pers.get(), log);
  return(pers.release());
}
