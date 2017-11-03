/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//------------------------------------------------------------------------------
#ifndef __TRIGSIMTRANSFORMS_MERGINGEVENTLOOPMGR_H_
#define __TRIGSIMTRANSFORMS_MERGINGEVENTLOOPMGR_H_
//------------------------------------------------------------------------------
// Gaudi/Athena include(s):
#include "GaudiKernel/MinimalEventLoopMgr.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IAlgExecStateSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "SGTools/DataStore.h"

#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"

//------------------------------------------------------------------------------
namespace TrigSim {
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
    class MergingEventLoopMgr : public MinimalEventLoopMgr {
        
    protected:
      /// Reference to the Algorithm Execution State Svc
      SmartIF<IAlgExecStateSvc>  m_aess;

    public:
        // Constructor
        MergingEventLoopMgr(const std::string& name, ISvcLocator* svcLoc);
        // Destructor
        virtual ~MergingEventLoopMgr();

        // Function initialising the event loop manager
        virtual StatusCode initialize();
        // Function finalising the event loop manager
        virtual StatusCode finalize();
        // Function running the event loop itself
        virtual StatusCode nextEvent(int maxEvt);
        // Function processing one input event
        virtual StatusCode executeEvent(void *par);
        // Function looping over one run (the same as nextEvent(int) for now
        virtual StatusCode executeRun(int maxEvt);
        // Function seeking to a given event (not implemented)
        virtual StatusCode seek(int evt);
        // Function returning the current event number
        virtual int curEvent();
        


        // Dealing with algorithms
        StatusCode initializeAlgorithms();
        StatusCode beginRunAlgorithms();
        StatusCode endRunAlgorithms();
        StatusCode executeAlgorithms();
   

        // Handle for the incident service
        ServiceHandle< IIncidentSvc > m_incidentSvc;
        
        
        // Handle for the two store gate services
        ServiceHandle< StoreGateSvc > m_primEvtStore;
        ServiceHandle< StoreGateSvc > m_secEvtStore;

        // Variables used to read the events from the RDO source:
        IEvtSelector*          m_primEvtSelector;
        IEvtSelector::Context* m_primEvtContext;
        StringProperty         m_primEvtSelName;

        // Variables used to read the events from the bytestream source:
        IEvtSelector*          m_secEvtSelector;
        IEvtSelector::Context* m_secEvtContext;
        StringProperty         m_secEvtSelName;


        // The run number of the current event
        EventID::number_type m_currentRun;
        
        // Flag showing if the next event will be the first one
        bool m_firstRun;
        
        // The number of events processed already
        int m_nEvt;

        // Object for writing messages
        mutable MsgStream m_log;

        EventContext m_eventContext;

        // Functionality for overwriting TriggerInfo
        BooleanProperty m_overwritePrimTriggerInfo;
        BooleanProperty m_overwriteSecTriggerInfo;

        StatusCode overwriteTriggerInfo(EventInfo *pFrom, EventInfo *pTo);


        // Share proxies between event stores.
        BooleanProperty m_sharePrimProxies;
        BooleanProperty m_shareSecProxies;
        BooleanProperty m_shareProxiesSafe;


        StatusCode loadObjects();

    }; // Class MergingEventLoopMgr
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
} // Namespace TrigSim
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------

