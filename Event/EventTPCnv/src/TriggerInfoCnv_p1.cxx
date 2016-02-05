/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "EventInfo/TriggerInfo.h"
#undef private
#undef protected

#include "EventTPCnv/TriggerInfoCnv_p1.h"


void TriggerInfoCnv_p1::transToPers(const TriggerInfo* , TriggerInfo_p1*, MsgStream &) 
{
    // Deprecated, p2 is used for writing
//     pers->m_extendedLevel1ID    = trans->m_extendedLevel1ID;
//     pers->m_level1TriggerType   = trans->m_level1TriggerType;
//     pers->m_level2TriggerInfo   = trans->m_level2TriggerInfo;
//     pers->m_eventFilterInfo     = trans->m_eventFilterInfo;
}


void TriggerInfoCnv_p1::persToTrans(const TriggerInfo_p1* pers, TriggerInfo* trans, MsgStream &)  
{
    trans->m_extendedLevel1ID     = pers->m_extendedLevel1ID;
    trans->m_level1TriggerType    = pers->m_level1TriggerType;
    // For backwards compatibility - add pers->m_level2TriggerInfo to
    // trans vector if not zero
    if (pers->m_level2TriggerInfo != 0) {
      trans->m_level2TriggerInfo.push_back(pers->m_level2TriggerInfo);
    }
    trans->m_eventFilterInfo      = pers->m_eventFilterInfo;
}


