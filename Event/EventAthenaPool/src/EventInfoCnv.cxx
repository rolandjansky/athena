/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EventInfoCnv.cxx
 * @brief Implementation file for AthenaPool converter for EventInfo 
 * @author Marcin.Nowak@cern.ch
 */

#include "EventInfo/EventType.h"
#include "EventInfoCnv.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventID.h"
#include "EventTPCnv/EventInfo_p1.h"
#include "EventTPCnv/EventInfoCnv_p1.h"
#include "EventTPCnv/EventInfoCnv_p2.h"
#include "EventTPCnv/EventInfoCnv_p3.h"
#include "EventTPCnv/EventInfoCnv_p4.h"

#include "AthenaKernel/IEvtIdModifierSvc.h"

static EventInfoCnv_p4   TPconverter_p4;
static EventInfoCnv_p3   TPconverter_p3;
static EventInfoCnv_p2   TPconverter_p2;
static EventInfoCnv_p1   TPconverter_p1;

EventInfoCnv::EventInfoCnv(ISvcLocator* svcloc) 
  : 
  EventInfoCnvBase(svcloc),
  m_checkedEventSelector(false),
  m_overrideRunNumber(false),
  m_simRunNumber(0),
  m_lumiBlockNumber(1),
  m_evtsPerLumiBlock(0),
  m_lbEvtCounter(0),
  m_timeStamp(0),
  m_timeStampInterval(0),
  m_evtIdModSvc(0)
{
  IService *svc = 0;
  bool createif = false;
  svcloc->getService("EvtIdModifierSvc", svc, createif).ignore();
  if (svc) {
    m_evtIdModSvc = dynamic_cast<IEvtIdModifierSvc*>(svc);
  }

}

EventInfo_PERS* EventInfoCnv::createPersistent(EventInfo* transObj) {
    MsgStream log(messageService(), "EventInfoCnv" ); 
    EventInfo_PERS *persObj = TPconverter_p4.createPersistent( transObj, log );
    log << MSG::DEBUG << "EventInfo write Success" << endreq;
    return persObj; 
}
    
EventInfo* EventInfoCnv::createTransient() {

    static pool::Guid   p4_guid("C634FDB6-CC4B-4BA2-B8F9-A84BE6A786C7");
    static pool::Guid   p3_guid("3E240CA8-5124-405B-9059-FAFC4C5954C6");
    static pool::Guid   p2_guid("22006E19-F0DA-4EFB-AF55-6FBDA421BF06");
    static pool::Guid   p1_guid("A3053CD9-47F4-4C0F-B73A-A6F93F5CC7B7");
    static pool::Guid   p0_guid("380D8BB9-B34F-470F-92CC-06C3D60F7BE4");
    if( compareClassGuid(p4_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        std::auto_ptr< EventInfo_p4 > col_vect( poolReadObject< EventInfo_p4 >() );
        MsgStream log(messageService(), "EventInfoCnv" );
        //log << MSG::DEBUG << "Reading EventInfo_p4" << endreq; 
        return massageEventInfo(TPconverter_p4.createTransient( col_vect.get(), log ));
    }
    else if( compareClassGuid(p3_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        std::auto_ptr< EventInfo_p3 > col_vect( poolReadObject< EventInfo_p3 >() );
        MsgStream log(messageService(), "EventInfoCnv" );
        //log << MSG::DEBUG << "Reading EventInfo_p3" << endreq; 
        return massageEventInfo(TPconverter_p3.createTransient( col_vect.get(), log ));
    }
    else if( compareClassGuid(p2_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        std::auto_ptr< EventInfo_p2 > col_vect( poolReadObject< EventInfo_p2 >() );
        MsgStream log(messageService(), "EventInfoCnv" );
        // log << MSG::DEBUG << "ILIJA Reading EventInfo_p2" << endreq; 
        return massageEventInfo(TPconverter_p2.createTransient( col_vect.get(), log ));
    }
    else if( compareClassGuid(p1_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        std::auto_ptr< EventInfo_p1 > col_vect( poolReadObject< EventInfo_p1 >() );
        MsgStream log(messageService(), "EventInfoCnv" );
        //log << MSG::DEBUG << "Reading EventInfo_p1" << endreq; 
        return massageEventInfo(TPconverter_p1.createTransient( col_vect.get(), log ));
    }
    else if( compareClassGuid(p0_guid) ) {
        // regular object from before TP separation, just return it
        EventInfo* ei =  poolReadObject< EventInfo >();

        // Fill MC event weight in transient EventType from
        // TriggerInfo. This is only needed for MC events: if the
        // weight is zero, and TriggerInfo exists - was stored at end
        // of eventFilterInfo
        if (ei->trigger_info() && 
            ei->event_type()->mc_event_weight() == 0 &&
            ei->trigger_info()->eventFilterInfo().size()) {
          ei->event_type()->set_mc_event_weight (ei->trigger_info()->eventFilterInfo().back());
        }
        else {
            // default weight = 1.0
            ei->event_type()->set_mc_event_weight(1.0);
        }
        return massageEventInfo(ei);
    } 
    throw std::runtime_error("Unsupported persistent version of EventInfo");
}


EventInfo*                
EventInfoCnv::massageEventInfo (EventInfo* ei)
{
    // We allow the run number, etc. to be modified EITHER by the
    // IEvtIdModifierSvc OR via the event selector via its
    // OverrideRunNumberFromInput property. Only one method is allowed
    // to modify EventInfo.

    if (m_evtIdModSvc) {
        // Modify EventInfo with the IEvtIdModifierSvc
        // Copy (save old) run/event numbers into EventType

        // We only do so if mc_channel_number has not already be set
        // either previously or elsewhere!!
        if (ei->event_type()->mc_channel_number() == 0) {
            ei->event_type()->set_mc_channel_number(ei->event_ID()->run_number());
            ei->event_type()->set_mc_event_number  (ei->event_ID()->event_number());
        }
        EventID* evtid = ei->event_ID();
        bool consume_stream = true;
        m_evtIdModSvc->modify_evtid(evtid, consume_stream);
        return (ei);
    }

    // Allow modification via the EventSelector:

    // When reading in simulation events, one may want to reset the
    // run number to allow conditions to be used with simulation
    // events. The run number is set via the (AthenaPool)EventSelector.

    // Check is reset is requested, if not return
    if (m_checkedEventSelector && !m_overrideRunNumber) return (ei);

    if (!m_checkedEventSelector) {
        // Get run number parameter from the EventSelector 
        m_checkedEventSelector = true;
        MsgStream log(messageService(), "EventInfoCnv" );
        log << MSG::DEBUG << "massageEventInfo: check if tag is set in jobOpts" << endreq;

        // Get name of event selector from the application manager to
        // make sure we get the one for MC signal events
        IProperty* propertyServer(0); 
        StatusCode sc = serviceLocator()->service("ApplicationMgr", propertyServer); 
        if (sc != StatusCode::SUCCESS ) {
            log << MSG::ERROR << "massageEventInfo: Cannot get ApplicationMgr " << endreq; 
            throw std::runtime_error("Cannot get ApplicationMgr");
        }
        StringProperty property("EvtSel", "");
        sc = propertyServer->getProperty(&property);
        if (!sc.isSuccess()) {
            log << MSG::ERROR << "unable to get EvtSel: found " << property.value() << endreq;
            throw std::runtime_error("Cannot get property EvtSel from the ApplicationMgr");
        }
        // Get EventSelector for ApplicationMgr
        std::string eventSelector = property.value();
        sc = serviceLocator()->service(eventSelector, propertyServer); 
        if (sc != StatusCode::SUCCESS ) {
            log << MSG::ERROR << "massageEventInfo: Cannot get EventSelector " << eventSelector << endreq; 
            throw std::runtime_error("Cannot get EventSelector");
        }
        BooleanProperty overrideRunNumber = IntegerProperty("OverrideRunNumberFromInput", false);
        sc = propertyServer->getProperty(&overrideRunNumber);
        if (!sc.isSuccess()) {
            // Not all EventSelectors have this property, so we must be tolerant
            ATH_MSG_INFO("massageEventInfo: unable to get OverrideRunNumberFromInput property from EventSelector ");
            return (ei);
        }
        m_overrideRunNumber = overrideRunNumber.value();
        if (m_overrideRunNumber) {
            IntegerProperty runNumber = IntegerProperty("RunNumber", 0);
            sc = propertyServer->getProperty(&runNumber);
            if (!sc.isSuccess()) {
                log << MSG::ERROR << "massageEventInfo: unable to get RunNumber from EventSelector: found " 
                    << runNumber.value()
                    << endreq;
                throw std::runtime_error("Cannot get RunNumber");
            }
            else {
                m_simRunNumber = (unsigned int)runNumber.value();
                log << MSG::DEBUG << "massageEventInfo: Run number:  " << m_simRunNumber 
                    << " obtained from " << eventSelector << endreq;
            }
            IntegerProperty lumiBlockNumber = IntegerProperty("FirstLB", 0);
            sc = propertyServer->getProperty(&lumiBlockNumber);
            if (!sc.isSuccess()) {
                log << MSG::INFO << "massageEventInfo: unable to get FirstLB from EventSelector. Using "
                    << m_lumiBlockNumber << endreq;
            }
            else {
                m_lumiBlockNumber = (unsigned int)lumiBlockNumber.value();
                log << MSG::DEBUG << "massageEventInfo: LumiBlock number:  " << m_lumiBlockNumber 
                    << " obtained from " << eventSelector << endreq;
            }
            IntegerProperty evtsPerLumiBlock = IntegerProperty("EventsPerLB", 0);
            sc = propertyServer->getProperty(&evtsPerLumiBlock);
            if (!sc.isSuccess()) {
                log << MSG::INFO << "massageEventInfo: unable to get EventsPerLB from EventSelector. Using "
                    << m_evtsPerLumiBlock << endreq;
            }
            else {
                m_evtsPerLumiBlock = (unsigned int)evtsPerLumiBlock.value();
                log << MSG::DEBUG << "massageEventInfo: EventsPerLB:  " << m_evtsPerLumiBlock 
                    << " obtained from " << eventSelector << endreq;
            }
            IntegerProperty timeStamp = IntegerProperty("InitialTimeStamp", 0);
            sc = propertyServer->getProperty(&timeStamp);
            if (!sc.isSuccess()) {
                log << MSG::INFO << "massageEventInfo: unable to get InitialTimeStamp from EventSelector. Using "
                    << m_timeStamp << endreq;
            }
            else {
                m_timeStamp = (unsigned int)timeStamp.value();
                log << MSG::DEBUG << "massageEventInfo: InitialTimeStamp:  " << m_timeStamp  
                    << " obtained from " << eventSelector << endreq;
            }
            IntegerProperty timeStampInterval = IntegerProperty("TimeStampInterval", 0);
            sc = propertyServer->getProperty(&timeStampInterval);
            if (!sc.isSuccess()) {
                log << MSG::INFO << "massageEventInfo: unable to get TimeStampInterval from EventSelector. Using "
                    << m_timeStampInterval << endreq;
            }
            else {
                m_timeStampInterval = (unsigned int)timeStampInterval.value();
                log << MSG::DEBUG << "massageEventInfo: TimeStampInterval:  " << m_timeStampInterval              
                    << " obtained from " << eventSelector << endreq;
            }
        }
        else {
            log << MSG::DEBUG << "massageEventInfo: OverrideRunNumberFromInput from EventSelector is false " 
                << endreq;
        }
    }

    if (m_overrideRunNumber) {
        // Reset run number, and copy run number into EventType

        // First make sure we're dealing with simulation events - if not, this is an error
        if (!(ei->event_type()->test(EventType::IS_SIMULATION))) {
            MsgStream log(messageService(), "EventInfoCnv" );
            log << MSG::ERROR << "massageEventInfo: trying to reset run number for data!!! " << endreq;
            log << MSG::ERROR << "    Job option EventSelector.RunNumber should NOT be set, found it set to: " << m_simRunNumber << endreq;
            throw std::runtime_error("Trying to reset run number for DATA!!!");
        }

        // Copy run/event numbers into EventType
        // We only do so if mc_channel_number has not already be set
        // either previously or elsewhere!!
        if (ei->event_type()->mc_channel_number() == 0) {
            ei->event_type()->set_mc_channel_number(ei->event_ID()->run_number());
            ei->event_type()->set_mc_event_number  (ei->event_ID()->event_number());
        }
        
        // Override run and other numbers
        if (m_evtsPerLumiBlock > 0) {
            m_lbEvtCounter++;
            if (m_lbEvtCounter > m_evtsPerLumiBlock) {
                m_lumiBlockNumber++;
                m_lbEvtCounter = 1;
            }
        }
        ei->event_ID()->set_run_number(m_simRunNumber);
        ei->event_ID()->set_lumi_block(m_lumiBlockNumber);
        ei->event_ID()->set_time_stamp(m_timeStamp);
        m_timeStamp += m_timeStampInterval;
    }
        

    return (ei);
}

    
        
        
    
