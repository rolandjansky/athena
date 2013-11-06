// this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file EventInfoCnv.h
 * @brief AthenaPool converter for EventInfo
 * @author Marcin.Nowak@cern.ch
 */

#ifndef EVENTATHENAPOOL_EVENTINFOCNV_H
#define EVENTATHENAPOOL_EVENTINFOCNV_H 1

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "EventInfo/EventInfo.h"
#include "EventTPCnv/EventInfo_p4.h"

// fwd declarations
class IEvtIdModifierSvc;

// the latest persistent representation type of EventInfo
typedef  EventInfo_p4  EventInfo_PERS;
typedef  T_AthenaPoolCustomCnv<EventInfo, EventInfo_PERS >   EventInfoCnvBase;

class EventInfoCnv : public EventInfoCnvBase {
    friend class CnvFactory<EventInfoCnv >;

protected:
    EventInfoCnv (ISvcLocator* svcloc);
    virtual EventInfo_PERS*   createPersistent (EventInfo* transObj);
    virtual EventInfo*        createTransient ();
    EventInfo*                massageEventInfo (EventInfo* ei);

private:

    bool           m_checkedEventSelector;
    bool           m_overrideRunNumber;
    unsigned int   m_simRunNumber;
    unsigned int   m_lumiBlockNumber;
    unsigned int   m_evtsPerLumiBlock;
    unsigned int   m_lbEvtCounter;
    unsigned int   m_timeStamp;
    unsigned int   m_timeStampInterval;

    IEvtIdModifierSvc* m_evtIdModSvc;
};



#endif

