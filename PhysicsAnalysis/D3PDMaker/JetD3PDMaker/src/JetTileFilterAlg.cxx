/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   JetTileFilterAlg.cxx
 * Author: Stephen Cole <stephen.cole@cern.ch>
 * 
 * Created on June 16, 2011, 1:30 PM
 */

#include "JetTileFilterAlg.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h" 
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"


JetTileFilterAlg::JetTileFilterAlg(const std::string& name, ISvcLocator* pSvcLocator):
        AthAlgorithm(name, pSvcLocator),
        m_jetsName("JetCollection"),
        m_jetsOut("FilteredJets"),
        m_jetEtaMin(-2.0),
        m_jetEtaMax(2.0),
        m_jetPhiMin(-4.0),
        m_jetPhiMax(4.0)
{
    
    //input jet collection SG key
    declareProperty("JetCollectionSGkey",m_jetsName);
    //output jet collection SG key
    declareProperty("JetCollOutSGkey",m_jetsOut);
        
    declareProperty("jetEtaMax",m_jetEtaMax);
        
    declareProperty("jetEtaMin",m_jetEtaMin);
        
    declareProperty("jetPhiMax",m_jetPhiMax);
        
    declareProperty("jetPhiMin",m_jetPhiMin);
    
}

JetTileFilterAlg::~JetTileFilterAlg() {
}

StatusCode JetTileFilterAlg::initialize(){
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << " in JetTileFilterAlg::initialize()" << endmsg; 
    
    StatusCode sc;
    
    // get StoreGate 
    
    // Find the detector store service.
    StoreGateSvc* detStore;
    sc = service("DetectorStore",detStore);
    if (sc.isFailure()) {
        log << MSG::ERROR
                << "Unable to get pointer to Detector Store Service" << endmsg;
        return sc;
    }
    
    return StatusCode::SUCCESS;
}

StatusCode JetTileFilterAlg::execute(){
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << " in JetTileFilterAlg::execute()" <<endmsg; 
    
    const JetCollection* jetcol;
    StatusCode sc=evtStore()->retrieve(jetcol,m_jetsName);
    
    if(sc!=StatusCode::SUCCESS){
        log<<MSG::ERROR<<"unable to retrieve Jet collection: "<<
                m_jetsName<<endmsg;
        return sc;
    }
    
    JetCollection* jout=new JetCollection(SG::VIEW_ELEMENTS);
    //JetCollectionHelper jhelper;
    //sc=jhelper.record_jets(evtStore(),jout, m_jetsOut);
    sc=evtStore()->record(jout, m_jetsOut);
    
    if(sc!=StatusCode::SUCCESS){
        log<<MSG::ERROR<<"unable to record Jet collection: "<<
                m_jetsOut<<endmsg;
        return sc;
    }
    
    JetCollection::const_iterator it;
    for(it=jetcol->begin();it<jetcol->end();++it){
        const Jet* jet=*it;
        if(jet->eta()>=m_jetEtaMin && 
                jet->eta()<m_jetEtaMax &&
                jet->phi()>=m_jetPhiMin &&
                jet->phi()<m_jetPhiMax) {
            Jet* jcopy=jet->clone(true,true);
            jout->push_back(jcopy);
        }
    }
    return sc;
}

StatusCode JetTileFilterAlg::finalize(){
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << " in JetTileFilterAlg::finalize()" <<endmsg; 
    return StatusCode::SUCCESS;    
}
