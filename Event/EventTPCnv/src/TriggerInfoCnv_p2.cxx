/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "EventInfo/TriggerInfo.h"
#undef private
#undef protected

#include "EventTPCnv/TriggerInfoCnv_p2.h"

void TriggerInfoCnv_p2::transToPers(const TriggerInfo* trans, TriggerInfo_p2* pers, MsgStream &) {

    pers->m_statusElement         = trans->m_statusElement;
    pers->m_extendedLevel1ID      = trans->m_extendedLevel1ID;
    pers->m_level1TriggerType     = trans->m_level1TriggerType;
    pers->m_level1TriggerInfo     = trans->m_level1TriggerInfo;
    pers->m_level2TriggerInfo     = trans->m_level2TriggerInfo;
    pers->m_eventFilterInfo       = trans->m_eventFilterInfo;
    pers->m_streamTags.resize(trans->m_streamTags.size());
    for (unsigned int i = 0; i < trans->m_streamTags.size(); ++i) {
	pers->m_streamTags[i].m_name           = trans->m_streamTags[i].m_name;
	pers->m_streamTags[i].m_type           = trans->m_streamTags[i].m_type;
	pers->m_streamTags[i].m_obeysLumiblock = trans->m_streamTags[i].m_obeysLumiblock;
    }
    
}

void TriggerInfoCnv_p2::persToTrans(const TriggerInfo_p2* pers, TriggerInfo* trans, MsgStream &)  {

    trans->m_statusElement        = pers->m_statusElement;
    trans->m_extendedLevel1ID     = pers->m_extendedLevel1ID;
    trans->m_level1TriggerType    = pers->m_level1TriggerType;
    trans->m_level1TriggerInfo    = pers->m_level1TriggerInfo;
    trans->m_level2TriggerInfo    = pers->m_level2TriggerInfo;
    trans->m_eventFilterInfo      = pers->m_eventFilterInfo;
    trans->m_streamTags.resize(pers->m_streamTags.size());
    for (unsigned int i = 0; i < pers->m_streamTags.size(); ++i) {
	trans->m_streamTags[i].m_name           = pers->m_streamTags[i].m_name;
	trans->m_streamTags[i].m_type           = pers->m_streamTags[i].m_type;
	trans->m_streamTags[i].m_obeysLumiblock = pers->m_streamTags[i].m_obeysLumiblock;
    }
}
