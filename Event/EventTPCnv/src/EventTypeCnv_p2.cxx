/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfo/EventType.h"
#include <iostream>
#include "EventTPCnv/EventTypeCnv_p2.h"
#include "EventTPCnv/vectorize.h"
#include "EventTPCnv/AtlasMcWeight.h"


void   EventTypeCnv_p2::persToTrans(std::vector<unsigned int>::const_iterator &i, EventType* trans, int version, bool bugcompat) const{
	// std::cout<<"persToTrans of EventType ..."<<std::endl;
        *trans = EventType();

        std::string user_type;
        UITostr(user_type, i, bugcompat);
        trans->set_user_type (user_type);

        std::vector<bool> bit_mask;
	UITobitmap(bit_mask,i, bugcompat);
        for (size_t ii = 0; ii < bit_mask.size(); ii++) {
          if (bit_mask[ii])
            trans->add_type (ii);
        }

        // we now decode here instead of in EventType
        float event_weight = (float)AtlasMcWeight::decode((*i)); ++i; 
        trans->set_mc_event_weight (event_weight, 0, 1);
	if(version>2){
		trans->set_mc_channel_number (*i); ++i;
		trans->set_mc_event_number   (*i); ++i;
	}
	// std::cout<<"persToTrans of EventType  ver: "<<version<<"\t weight:"<<trans->m_mc_event_weight<<std::endl;
	
}

void   EventTypeCnv_p2::transToPers(const EventType* trans, std::vector<unsigned int> &pers) const {
    // Deprecated - writing out with _p3 RDS 2013/03

// 	// std::cout<<"transToPers of EventType ..."<<std::endl;
        strToUI(trans->user_type_raw(),pers);
 	bitmapToUI(trans->bit_mask(),pers);
        double wt = 0;
        if (trans->n_mc_event_weights() > 0)
          wt     = trans->mc_event_weight(0);
 	pers.push_back(AtlasMcWeight::encode(wt));
 	pers.push_back(trans->mc_channel_number());
 	pers.push_back(trans->mc_event_number());
        
// 	// std::cout<<"transToPers of EventType. weight: "<<trans->m_mc_event_weight<<std::endl;
}
