/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "EventInfo/TriggerInfo.h"
#undef private
#undef protected

#include "EventTPCnv/TriggerInfoCnv_p3.h"
#include "EventTPCnv/vectorize.h"

void TriggerInfoCnv_p3::persToTrans(std::vector<unsigned int>::const_iterator &i, TriggerInfo* trans, bool bugcompat){
    trans->m_statusElement           = (*i); ++i;
    trans->m_extendedLevel1ID        = (*i); ++i;
    trans->m_level1TriggerType       = (*i); ++i;
	
//	std::cout<<"persToTrans of TriggerInfo\t"<<trans->m_level1TriggerType<<std::endl;
	
    unsigned int size=(*i); ++i;
    trans->m_level1TriggerInfo.resize(size);
    for (unsigned int w=0;w<size;w++) {trans->m_level1TriggerInfo[w]=(*i); ++i;}
    size=(*i); ++i;
    trans->m_level2TriggerInfo.resize(size);
    for (unsigned int w=0;w<size;w++) {trans->m_level2TriggerInfo[w]=(*i); ++i;}
    size=(*i); ++i;
    trans->m_eventFilterInfo.resize(size);
    for (unsigned int w=0;w<size;w++) {trans->m_eventFilterInfo[w]=(*i); ++i;}
	
//	std::cout<<"persToTrans of TriggerInfo\t"<<trans->m_eventFilterInfo.size()<<std::endl;
    size=(*i); ++i;
    trans->m_streamTags.resize(size);
    for (unsigned int w=0;w<size;w++) {
      UITostr(trans->m_streamTags[w].m_name,i, bugcompat);
      UITostr(trans->m_streamTags[w].m_type,i, bugcompat);
    }
    std::vector<bool> t;
    UITobitmap(t, i, bugcompat);
    for (unsigned int w=0;w<size;w++) {
        trans->m_streamTags[w].m_obeysLumiblock=t[w];
//	std::cout<<"persToTrans of TriggerInfo \t"<<trans->m_streamTags[w].m_obeysLumiblock<<std::endl;
    }
}

void TriggerInfoCnv_p3::transToPers(const TriggerInfo* trans, std::vector<unsigned int> &pers){
    pers.push_back(trans->m_statusElement);
    pers.push_back(trans->m_extendedLevel1ID);
    pers.push_back(trans->m_level1TriggerType);
    // std::cout<<"transToPers of TriggerInfo \t"<<trans->m_level1TriggerType<<std::endl;
    pers.push_back(trans->m_level1TriggerInfo.size());
    std::copy (trans->m_level1TriggerInfo.begin(), trans->m_level1TriggerInfo.end(), std::back_inserter (pers));
    pers.push_back(trans->m_level2TriggerInfo.size());
    std::copy (trans->m_level2TriggerInfo.begin(), trans->m_level2TriggerInfo.end(), std::back_inserter (pers));
    pers.push_back(trans->m_eventFilterInfo.size());
    std::copy (trans->m_eventFilterInfo.begin(), trans->m_eventFilterInfo.end(), std::back_inserter (pers));
	
    // std::cout<<"transToPers of TriggerInfo \t"<<trans->m_eventFilterInfo.size()<<std::endl;
    pers.push_back(trans->m_streamTags.size());
    std::vector<bool> t; t.reserve(trans->m_streamTags.size());
    for (unsigned int i = 0; i < trans->m_streamTags.size(); ++i) {
        strToUI(trans->m_streamTags[i].m_name,pers);
        strToUI(trans->m_streamTags[i].m_type,pers);
        t.push_back(trans->m_streamTags[i].m_obeysLumiblock);
        // std::cout<<"transToPers of TriggerInfo "<<trans->m_streamTags[i].m_obeysLumiblock<<std::endl;
    }
    bitmapToUI(t, pers);
	
}


