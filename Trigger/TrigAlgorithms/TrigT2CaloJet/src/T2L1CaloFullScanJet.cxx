/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2L1CaloFullScanJet.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet
//
// AUTHOR:   Matthew Tamsett
// ********************************************************************


#include "GaudiKernel/IToolSvc.h"


#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEvent/TrigT2JetContainer.h"
#include "TrigT2CaloJet/T2L1CaloFullScanJet.h"
#include "TrigT2CaloJet/T2L1CaloJetFullScanBaseTool.h"
#include "TrigTimeAlgs/TrigTimer.h"

#include <sys/time.h>

#include <math.h>

class ISvcLocator;

T2L1CaloFullScanJet::T2L1CaloFullScanJet(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
    m_jets(0),
    m_tools(this)

{   declareProperty("T2JetTools",   m_tools, "L1.5 Full Scan Tools");
    declareProperty("jetOutputKey", m_jetOutputKey = "TrigT2CaloJet");
    // debug
    declareProperty("recordOutput", m_recordOutput = true);
    
    declareMonitoredVariable("nJets",          m_nJets);
    declareMonitoredVariable("nTowers",        m_nTowers);
    declareMonitoredVariable("UnpckTime",      m_UnpckTime);
    declareMonitoredVariable("L1_UnpckTime",   m_L1_UnpckTime);
    declareMonitoredVariable("FastJetTime",    m_FastJetTime);
    declareMonitoredVariable("JetFindingTime", m_JetFindingTime);
    declareMonitoredVariable("OverallTime",    m_TotalTime);
    declareMonitoredVariable("RoITime",        m_RoITime);
    declareMonitoredStdContainer("E",    m_e);
    declareMonitoredStdContainer("Et",   m_et);
    declareMonitoredStdContainer("Eem",  m_eem);
    declareMonitoredStdContainer("Ehad", m_ehad);
    declareMonitoredStdContainer("Etem", m_etem);
    declareMonitoredStdContainer("Ethad",m_ethad);
    declareMonitoredStdContainer("Eta",  m_eta);
    declareMonitoredStdContainer("Phi",  m_phi);
    declareMonitoredStdContainer("Emf",  m_emfrac);
    declareMonitoredStdContainer("nLeadingTowers", m_nLeadingTowers);
}

HLT::ErrorCode T2L1CaloFullScanJet::hltInitialize()
{
    m_log = new MsgStream(msgSvc(), name());
    
    if((*m_log).level() <= MSG::INFO){
        (*m_log) << MSG::INFO << " Initalizing FEX algorithm: " << name() << endmsg;
        (*m_log) << MSG::DEBUG << "Options: " << endmsg;
        (*m_log) << MSG::DEBUG << "   jetOutputKey:     " << m_jetOutputKey << endmsg;
        (*m_log) << MSG::DEBUG << "   recordOutput:     " << m_recordOutput << endmsg;
    }
    
    if (timerSvc()) {
        m_timer.push_back(addTimer("TimeTot"));    
        m_timer.push_back(addTimer("RoIMaking"));
    }
    
    if ( m_tools.retrieve().isFailure() ) {
        (*m_log) << MSG::ERROR << "Failed to retreive helper tools: " << m_tools << endmsg;
    } else {
        (*m_log) << MSG::DEBUG << "Retrieved l1.5 full scan tools: " << m_tools << endmsg;
    }
    
    // add timers for the tools
    if (timerSvc()) {
        ToolHandleArray< T2L1CaloJetFullScanBaseTool >::iterator itTool = m_tools.begin(),
        itToolEnd = m_tools.end();
        for (  ; itTool != itToolEnd; ++itTool ) {
            if((*m_log).level() <= MSG::DEBUG)
                (*m_log) << MSG::DEBUG <<"Tool with name " << (*itTool).name()<< endmsg;
            
            m_timer.push_back(addTimer((*itTool).name()));
        }
    }
    
    m_jets = new DataVector<TrigT2Jet>(SG::VIEW_ELEMENTS);
    m_jets->reserve(100);
    
    m_e.reserve(100);
    m_et.reserve(100);
    m_eem.reserve(100);
    m_ehad.reserve(100);
    m_etem.reserve(100);
    m_ethad.reserve(100);
    m_eta.reserve(100);
    m_phi.reserve(100);
    m_emfrac.reserve(100);
    m_nLeadingTowers.reserve(100);
    
    return HLT::OK;
}

T2L1CaloFullScanJet::~T2L1CaloFullScanJet(){
}

HLT::ErrorCode T2L1CaloFullScanJet::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& /*tes_in*/,
                                               unsigned int type_out)
{
    // since this is an AllTEAlgo, we have to call the monitoring ourselves:
    beforeExecMonitors().ignore();
    
    if(timerSvc()) m_timer[0]->start();      
    
    m_nJets = -99;
    m_nTowers = -99;
    m_UnpckTime = -99;
    m_L1_UnpckTime = -99;
    m_FastJetTime = -99;
    m_JetFindingTime = -99;
    m_TotalTime = -99;
    m_RoITime = -99;
    m_e.clear();
    m_et.clear();
    m_eem.clear();
    m_ehad.clear();
    m_etem.clear();
    m_ethad.clear();
    m_eta.clear();
    m_phi.clear();
    m_emfrac.clear();
    m_nLeadingTowers.clear();
    m_jets->clear();

#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << "================= Executing T2L1CaloFullScanJet FEX " << name() << endmsg;
        //(*m_log) << MSG::DEBUG << "m_jetOutputKey: " << m_jetOutputKey << endmsg;
    }
#endif

    DataVector<TrigT2Jet>::const_iterator begin_jet, end_jet; // iterators for the produced jets
  
    // ====== Run the tools
    // do loop over tools
    ToolHandleArray< T2L1CaloJetFullScanBaseTool >::iterator itTool = m_tools.begin(),
                                                          itToolEnd = m_tools.end();
    
    int currant_tool=0;
    //uint32_t error = 0;
  
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << "Looping over tools vector" << endmsg;
    }
#endif
  
    for (  ; itTool != itToolEnd; ++itTool ) {
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            (*m_log) << MSG::DEBUG << "Running tool: [" << currant_tool+1 << "] of " << m_tools.size() << endmsg;
            (*m_log) << MSG::DEBUG << "Alg name: " << (*itTool)->name() << endmsg;
        } 
#endif
        
        if (timerSvc()) m_timer[currant_tool+2]->start(); // offset = number of other timers
    
        if ((*itTool)->execute(m_jets,m_nTowers,m_L1_UnpckTime,m_UnpckTime,m_FastJetTime,m_JetFindingTime).isFailure()){
            if((*m_log).level() <= MSG::DEBUG){
                (*m_log) << MSG::WARNING << "T2L1CaloJet AlgTool returned Failure" << endmsg;
            }
            return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,HLT::Reason::USERDEF_1);
        }
    
        //uint32_t in_error = (*itTool)->report_error();
        //error|=in_error;
    
        if (timerSvc()) m_timer[currant_tool+2]->stop();
        currant_tool++;
    }
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << "=== Finished jet finding" << endmsg;
        (*m_log) << MSG::DEBUG << " Towers unpacked:  " << m_nTowers << " in " << m_UnpckTime << " ms, of this L1 unpacking took "<< m_L1_UnpckTime << " ms" << endmsg;
        (*m_log) << MSG::DEBUG << " T2CaloJets found: " << m_jets->size() << " in " << m_JetFindingTime << " ms, of this FastJet took " << m_FastJetTime << " ms" << endmsg; 
        if (m_UnpckTime > 50){
            (*m_log) << MSG::DEBUG << " That was a long time " << endmsg;
        }
        
        begin_jet = m_jets->begin();
        end_jet = m_jets->end();        
        int jet_counter = 0;
        for (; begin_jet != end_jet; ++begin_jet){
            (*m_log) << MSG::DEBUG << " T2CaloJet["<<jet_counter
                                   << "]: Et: " << (*begin_jet)->et() 
                                   << " MeV, eta: " << (*begin_jet)->eta() 
                                   << ", phi; " << (*begin_jet)->phi() << endmsg;
            jet_counter++;
        }
    }
#endif
    // ====== now monitor record and attach features
  
    if(timerSvc()) m_timer[1]->start(); // start of RoI recording
  
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << "Monitoring, recording and attaching features" << endmsg;
        (*m_log) << MSG::DEBUG << "Using L2 output jet key = " << m_jetOutputKey << endmsg;
    }
#endif
  
    m_nJets = m_jets->size();
    
    // store the final jets into an output TE. 
    HLT::TriggerElement* outputTE = 0;
    std::string key = "";
    int recorded_jet_counter = 0;
    if (m_recordOutput){        
      outputTE = addRoI(type_out, new TrigRoiDescriptor(true));
      outputTE->setActiveState(true);  
                
    } else {
        //outputTE->setActiveState(false);
    }
    
    begin_jet = m_jets->begin();
    end_jet = m_jets->end();
    for (; begin_jet != end_jet; ++begin_jet){
        m_jet = (*begin_jet);
        
        // Monitor
        m_e.push_back( m_jet->e()); 
        m_et.push_back( m_jet->et()); 
        m_eta.push_back( m_jet->eta() ); 
        m_phi.push_back( m_jet->phi() ); 
        m_eem.push_back( m_jet->eem0()); 
        m_ehad.push_back( m_jet->ehad0()); 
        m_etem.push_back( (m_jet->eem0()/cosh(m_jet->eta()))); 
        m_ethad.push_back( (m_jet->ehad0()/cosh(m_jet->eta()))); 
        m_emfrac.push_back( m_jet->emf() ); 
        m_nLeadingTowers.push_back( m_jet->nLeadingCells() );
      
        if (m_recordOutput){
#ifndef NDEBUG
            if((*m_log).level() <= MSG::DEBUG){
                (*m_log) << MSG::DEBUG << " recording T2CaloJet["<<recorded_jet_counter<<"]"<<endmsg;
            }
#endif
            // store each jet as we go along so they're in the standard T2JetContainer
          
            HLT::ErrorCode hltStatus = recordAndAttachFeature(outputTE, m_jet, key, m_jetOutputKey);
            if (hltStatus != HLT::OK){
                (*m_log) << MSG::ERROR << "recording of TrigT2Jet into StoreGate failed" << endmsg;
                return hltStatus;
            }
      
            recorded_jet_counter++;
        }
      
        
      
    }
  
  
    if(timerSvc()) m_timer[1]->stop(); // end of RoI recording
    
    if(timerSvc()) m_timer[0]->stop(); // end of execute    
    

#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << "RoI making time: " << m_timer[1]->elapsed() << " ms " << endmsg;
        (*m_log) << MSG::DEBUG << "Total time: " << m_timer[0]->elapsed() << " ms " << endmsg;
        if (m_timer[0]->elapsed() > 50){
            (*m_log) << MSG::DEBUG << "That was a long time " << endmsg;
        }
        (*m_log) << MSG::DEBUG << "================= Finished T2L1CaloFullScanJet FEX " << name() << endmsg;
    }
#endif
  
    m_TotalTime = m_timer[0]->elapsed();
    m_RoITime = m_timer[1]->elapsed();
    
    // since this is an AllTEAlgo, we have to call the monitoring ourselves:
    afterExecMonitors().ignore();
    
    return HLT::OK;
}


HLT::ErrorCode T2L1CaloFullScanJet::hltFinalize()
{
    if ( (*m_log).level() <= MSG::DEBUG )
        (*m_log) << MSG::DEBUG << "Finalizing T2L1CaloFullScanJet FEX " << name() << endmsg;
    
    m_jets->clear();
    delete m_jets;
    m_timer.clear();
    m_e.clear();
    m_et.clear();
    m_eta.clear();
    m_phi.clear();
    m_eem.clear();
    m_ehad.clear();
    m_emfrac.clear();
    m_nLeadingTowers.clear();
    
    delete m_log;
      
    return HLT::OK;
}

