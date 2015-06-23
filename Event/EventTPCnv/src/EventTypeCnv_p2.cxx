/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "EventInfo/EventType.h"
#undef private
#undef protected
#include <iostream>
#include "EventTPCnv/EventTypeCnv_p2.h"
#include "EventTPCnv/vectorize.h"
#include "EventTPCnv/AtlasMcWeight.h"


void   EventTypeCnv_p2::persToTrans(std::vector<unsigned int>::const_iterator &i, EventType* trans, int version, bool bugcompat){
	// std::cout<<"persToTrans of EventType ..."<<std::endl;
        UITostr(trans->m_user_type, i, bugcompat);
	UITobitmap(trans->m_bit_mask,i, bugcompat);
        // we now decode here instead of in EventType
	trans->m_mc_event_weights[0] = (float)AtlasMcWeight::decode((*i)); ++i; 
	if(version>2){
		trans->m_mc_channel_number = (*i); ++i;
		trans->m_mc_event_number   = (*i); ++i;
	}
	// std::cout<<"persToTrans of EventType  ver: "<<version<<"\t weight:"<<trans->m_mc_event_weight<<std::endl;
	
}

void   EventTypeCnv_p2::transToPers(const EventType* /*trans*/, std::vector<unsigned int> &/*pers*/){
    // Deprecated - writing out with _p3 RDS 2013/03

// 	// std::cout<<"transToPers of EventType ..."<<std::endl;
// 	strToUI(trans->m_user_type,pers);
// 	bitmapToUI(trans->m_bit_mask,pers);
// 	pers.push_back(trans->m_mc_event_weight);
// 	pers.push_back(trans->m_mc_channel_number);
// 	pers.push_back(trans->m_mc_event_number);
        
// 	// std::cout<<"transToPers of EventType. weight: "<<trans->m_mc_event_weight<<std::endl;
}
