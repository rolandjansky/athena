/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfo/TriggerInfo.h"
#include "EventTPCnv/TriggerInfoCnv_p2.h"

void TriggerInfoCnv_p2::transToPers(const TriggerInfo* trans, TriggerInfo_p2* pers, MsgStream &) const {

    pers->m_statusElement         = trans->statusElement();
    pers->m_extendedLevel1ID      = trans->extendedLevel1ID();
    pers->m_level1TriggerType     = trans->level1TriggerType();
    pers->m_level1TriggerInfo     = trans->level1TriggerInfo();
    pers->m_level2TriggerInfo     = trans->level2TriggerInfo();
    pers->m_eventFilterInfo       = trans->eventFilterInfo();
    size_t sz = trans->streamTags().size();
    pers->m_streamTags.resize(sz);
    for (unsigned int i = 0; i < sz; ++i) {
        const TriggerInfo::StreamTag& st = trans->streamTags()[i];
	pers->m_streamTags[i].m_name           = st.name();
	pers->m_streamTags[i].m_type           = st.type();
	pers->m_streamTags[i].m_obeysLumiblock = st.obeysLumiblock();
    }
    
}

void TriggerInfoCnv_p2::persToTrans(const TriggerInfo_p2* pers, TriggerInfo* trans, MsgStream &)  const {

    trans->setStatusElement       (pers->m_statusElement);
    trans->setExtendedLevel1ID    (pers->m_extendedLevel1ID);
    trans->setLevel1TriggerType   (pers->m_level1TriggerType);
    trans->setLevel1TriggerInfo   (pers->m_level1TriggerInfo);
    trans->setLevel2TriggerInfo   (pers->m_level2TriggerInfo);
    trans->setEventFilterInfo     (pers->m_eventFilterInfo);
    std::vector<TriggerInfo::StreamTag> st;
    st.reserve(pers->m_streamTags.size());
    for (unsigned int i = 0; i < pers->m_streamTags.size(); ++i) {
        st.emplace_back (pers->m_streamTags[i].m_name,
                         pers->m_streamTags[i].m_type,
                         pers->m_streamTags[i].m_obeysLumiblock);
    }
    trans->setStreamTags (std::move (st));
}

TriggerInfo* TriggerInfoCnv_p2::createTransientConst (const TriggerInfo_p2* persObj, MsgStream& log) const
{
  auto trans = std::make_unique<TriggerInfo>();
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}
