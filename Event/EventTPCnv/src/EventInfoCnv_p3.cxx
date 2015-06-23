/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "EventInfo/EventInfo.h"
#undef private
#undef protected

#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"

#include "EventTPCnv/EventInfoCnv_p3.h"
#include "EventTPCnv/EventIDCnv_p2.h"
#include "EventTPCnv/EventTypeCnv_p2.h"
#include "EventTPCnv/TriggerInfoCnv_p3.h"

static	EventIDCnv_p2		idConv;
static	EventTypeCnv_p2		typeConv;
static	TriggerInfoCnv_p3	trigInfoCnv;


void EventInfoCnv_p3::persToTrans(const EventInfo_p3* pers, EventInfo* trans, MsgStream &/* log */)  {
    std::vector<unsigned int>::const_iterator i= pers->m_AllTheData.begin();
	
    int vers = (*i); i++;
    // std::cout<<"EventInfo vers  EventID: "<<(vers&0x003f)<<"\t EventType: "<< ((vers>>6)&0x003f)<<"\tTriggerInfo: "<<((vers>>12)&0x003f) <<"\t Local vars: "<<(vers>>18)<<std::endl;

    bool bugcompat = (vers&(1<<24)) == 0;

    idConv.persToTrans (i, trans->m_event_ID);
    typeConv.persToTrans(i, trans->m_event_type, (vers>>6)&0x003f, bugcompat );
	
    if ((vers>>12)&0x003f){ // if there is trigger info
        trigInfoCnv.persToTrans(i, trans->m_trigger_info, bugcompat);
    }
    else
      *trans->m_trigger_info = TriggerInfo();

    if (i != pers->m_AllTheData.end()) {
        // must reset event flags, in case we're reusing the EventInfo object
        trans->m_event_flags.clear();
        
        unsigned int n = pers->m_AllTheData.size() - (i - pers->m_AllTheData.begin());
        trans->m_event_flags.reserve(n);
    }
    while(i!=pers->m_AllTheData.end()){ // there is still some data: m_event_flags
        trans->m_event_flags.push_back((*i)); i++;
    }
    // std::cout<<"persToTrans of EventInfo \t"<<trans->m_event_flags.size()<<std::endl;

}

void EventInfoCnv_p3::transToPers(const EventInfo* trans, EventInfo_p3* pers, MsgStream &/* log */) {
	
    // std::cout<<"transToPers of EventInfo ..."<<std::endl;
    pers->m_AllTheData.reserve(34);

    // For versioning of the different elements, we use 6 bits for
    // each part:
    //   the first number (LSB) is version of EventIDCnv
    //   the second number is EventType version
    //   the third is TriggerInfo version
    //   the fourth is version of local variables ( now just m_event_flags)
    // version number == 0 means that part is not saved at all
    // bit 24 is set if the vector packing bug is fixed.
	
    int versionToWrite = 2 | (3<<6) | (3<<12) | (1<<18) | (1<<24);
    pers->m_AllTheData.push_back(versionToWrite);
    idConv.transToPers(trans->m_event_ID, pers->m_AllTheData);
    typeConv.transToPers(trans->m_event_type, pers->m_AllTheData);
    // 
    if( trans->m_trigger_info ) { // this should always be there !
        trigInfoCnv.transToPers(trans->m_trigger_info, pers->m_AllTheData);
    }else{
        pers->m_AllTheData[0]&=0xfffc0fff; // sets TriggerInfo version to 0
    }
		
    // Don't write out event flags if they are all == 0
    if (trans->m_event_flags.size()) {
        bool copy = false;
        for (unsigned int i = 0; i < trans->m_event_flags.size(); ++i) {
            // std::cout << "event flag: \t" <<  std::hex << trans->m_event_flags[i] << std::dec << std::endl;
            if (trans->m_event_flags[i]) { copy = true; break; }
        }
        if (copy) {
            std::copy (trans->m_event_flags.begin(), trans->m_event_flags.end(), std::back_inserter (pers->m_AllTheData)); 
            // std::cout << "event flags written: \t" << trans->m_event_flags.size() << std::endl;
        }
    }
    // std::cout << "transToPers of EventInfo out size \t" << pers->m_AllTheData.size() << std::endl;
}




// work around the default constructor of EventInfo allocating memory
EventInfo* EventInfoCnv_p3::createTransient( const EventInfo_p3* persObj, MsgStream &log) {
    std::auto_ptr<EventInfo> trans( new EventInfo() );
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}
