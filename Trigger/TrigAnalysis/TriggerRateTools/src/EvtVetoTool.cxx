/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerRateTools/EvtVetoTool.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/MsgStream.h"

#include <iostream>
#include <fstream>
#include <stdio.h>

EvtVetoTool::EvtVetoTool (const std::string& type, const std::string& name, const IInterface* parent) : 
        AlgTool( type, name, parent ), 
        m_log(0),
        m_storeGate("StoreGateSvc", name) {
    
    declareProperty("TruthJetsContainer", m_truthJetsContainerName = "Cone7TruthJets");
    declareProperty("VetoThreshold", m_vetoThreshold = 0.0);
    declareInterface<EvtVetoTool>(this);
    declareInterface<IEvtVetoTool>(this);
  
}

/**************/
/* Destructor */
/**************/

EvtVetoTool::~EvtVetoTool() {

}

/**************/
/* Initialize */
/**************/

StatusCode EvtVetoTool::initialize() {
    
    // get message service and print out properties
    m_log = new MsgStream(msgSvc(), name());

    (*m_log) << MSG::INFO << "In initialize()" << endreq;
    
    // get StoreGate
    StatusCode sc = m_storeGate.retrieve();

    if ( sc.isFailure() ) {
        (*m_log) << MSG::ERROR << "Could not retrieve StoreGateSvc!" << endreq;
        return sc;
    }

    return StatusCode::SUCCESS;
}

/************/
/* Finalize */
/************/

StatusCode EvtVetoTool::finalize() {

    (*m_log) << MSG::INFO << "In finalize()" << endreq;

    return StatusCode::SUCCESS;
}

bool EvtVetoTool::isPassed() {

    const JetCollection* mcjetTES = 0 ;
    m_truthJets.clear();
    StatusCode sc = m_storeGate->retrieve( mcjetTES, m_truthJetsContainerName );

    if ( sc.isFailure() ) {
        (*m_log) << MSG::ERROR << "Could not retrieve truth jet collection" << endreq;
    }

    (*m_log) << MSG::INFO << "Checking that event passed veto criteria" << endreq;
    JetCollection::const_iterator truItrB = mcjetTES->begin();
    JetCollection::const_iterator truItrE = mcjetTES->end();
    for ( JetCollection::const_iterator truItr=truItrB ; truItr != truItrE ; ++ truItr ) {
        m_truthJets.push_back( *truItr ) ;
        (*m_log) << MSG::DEBUG << "Found truth jet with " << (*truItr)->pt() << " in pt" << endreq;
        if ((*truItr)->pt() > m_vetoThreshold) {
            (*m_log) << MSG::INFO << "Event passed veto" << endreq;
            return true;
        }
    }
    
    (*m_log) << MSG::INFO << "Event did not pass veto" << endreq;

    return false;
}
