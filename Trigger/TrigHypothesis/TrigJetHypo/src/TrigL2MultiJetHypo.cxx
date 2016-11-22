/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigL2MultiJetHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigMultiJetHypo
//
// AUTHOR:
//
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

//#include "TrigSteeringEvent/TriggerElement.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEvent/TrigT2JetContainer.h" // Not needed anymore

#include "TrigJetHypo/TrigL2MultiJetHypo.h"

#include "CLHEP/Units/SystemOfUnits.h"


class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigL2MultiJetHypo::TrigL2MultiJetHypo(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {
    
    declareProperty("jetInputKey",      m_jetInputKey = "TrigT2CaloJet");
    declareProperty("Etcut_L2",         m_EtCut_L2 = 20*CLHEP::GeV, "cut value for L2 jet et"); // Default: 20 GeV
    declareProperty("Multiplicity_L2",  m_multiplicitycut = 1, "cut value for L2 jet multiplicity"); // Default: 1
    declareProperty("EtThresholds",     m_EtThresholdsInput ); // Default: 40 GeV
    declareProperty("etaMinCut",        m_etaMinCut    = 0.); // Default: 0
    declareProperty("etaMaxCut",        m_etaMaxCut    = 3.2); // Default: 3.2 - central jets
    
    declareProperty("doMonitoring_L2",  m_doMonitoring = false, "switch on/off monitoring" );
    declareProperty("AcceptAll",        m_acceptAll=false);
    //declareProperty("histoPath", m_path = "/stat/Monitoring/EventFilter" );
    
    declareMonitoredVariable("CutCounter",         m_cutCounter);
    declareMonitoredVariable("nJets",              m_nJets);
    declareMonitoredStdContainer("E",              m_e);
    declareMonitoredStdContainer("Et",             m_et);
    declareMonitoredStdContainer("Eta",            m_eta);
    declareMonitoredStdContainer("Phi",            m_phi);
    declareMonitoredStdContainer("nLeadingTowers", m_nLeadingTowers);
  
  
}
/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigL2MultiJetHypo::~TrigL2MultiJetHypo()
{ }

// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2MultiJetHypo::hltInitialize()
// ----------------------------------------------------------------------
{
    m_log = new MsgStream(msgSvc(), name());
    (*m_log) << MSG::INFO << "in initialize(): " << name() << endmsg;
    
    // Initialize timing service
    //------------------------------
    if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
        (*m_log) << MSG::WARNING << name() << ": Unable to locate TrigTimer Service" << endmsg;
    }
    if (m_timersvc) {    
        TrigTimer* tmp = m_timersvc->addItem("TrigL2MultiJetHypo.TrigL2MultiJetHypoTot");
        m_timers.push_back(tmp);
    }
    
    if ( m_EtThresholdsInput.empty() ) { 
        for ( unsigned i=0 ; i<m_multiplicitycut ; i++ ) m_EtThresholds.push_back(m_EtCut_L2);
    } else { 
        for  ( unsigned i=0 ; i<m_EtThresholdsInput.size() ; i++ ) m_EtThresholds.push_back(m_EtThresholdsInput[i]);    
        if ( m_EtThresholds.size()!=m_multiplicitycut ) {
            /// if not enough thresholds for the required number of jets... 
            
            /// pad with zeros to the required number of thresholds just in case...    
            for  ( unsigned i=m_EtThresholds.size() ; i<m_multiplicitycut ; i++ ) m_EtThresholds.push_back(0);
            
            (*m_log) << MSG::ERROR << name()
                << ": mismatch between number of jets required and number of thresholds: " << m_multiplicitycut 
                << " jets requested but only " << m_EtThresholdsInput.size() << " provided." << endmsg;    
            
            return HLT::ErrorCode(HLT::Action::ABORT_JOB,HLT::Reason::BAD_JOB_SETUP);
        }
        
        bool sorted = true;
        for  ( unsigned i=1 ; i<m_EtThresholds.size() ; i++ ) if ( m_EtThresholds[i]>m_EtThresholds[i-1] ) { 
            sorted = false;
            (*m_log) << MSG::WARNING << name()
                << ": Threshold " << i << " (" << m_EtThresholds[i] 
                << " GeV) greater that threshold " << i-1 << " (" << m_EtThresholds[i-1]
                << " GeV), thresholds should be in DECREASING order, highest first;  ET1>=ET2>=..." << endmsg;    
            
            // return HLT::ErrorCode(HLT::Action::ABORT_JOB,HLT::Reason::BAD_JOB_SETUP);
        }
        
        if  ( !sorted ) { 
            (*m_log) << MSG::WARNING << name() << ": sorting thresholds into reverse order" << endmsg; 
            std::sort( m_EtThresholds.begin(), m_EtThresholds.end() );
            std::reverse( m_EtThresholds.begin(), m_EtThresholds.end() );
        }
    }
    
    for ( unsigned i=0 ; i<m_EtThresholds.size() ; i++ ) {
        (*m_log) << MSG::INFO << name() 
            << "\t EtThreshold[" << i 
            << "]=" << m_EtThresholds[i] << " GeV" 
            << endmsg;
    }
    
    (*m_log) << MSG::INFO << "\t eta cuts, min: " << m_etaMinCut << ", max: " << m_etaMaxCut << endmsg; 
    
    m_accepted_L2=0;
    m_rejected_L2=0;
    m_errors_L2=0;
    
    m_e.reserve(100);
    m_et.reserve(100);
    m_eta.reserve(100);
    m_phi.reserve(100);
    m_nLeadingTowers.reserve(100);
    m_accepted_jets.reserve(100);


  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2MultiJetHypo::hltFinalize(){
// ----------------------------------------------------------------------
    
    (*m_log) << MSG::INFO << "in finalize()" << endmsg;
    (*m_log) << MSG::INFO << "Events (Lvl2) accepted/rejected/errors:  "<< m_accepted_L2 <<" / "<<m_rejected_L2<< " / "<< m_errors_L2<< endmsg;
    m_e.clear();
    m_et.clear();
    m_eta.clear();
    m_phi.clear();
    m_nLeadingTowers.clear();
    m_accepted_jets.clear();
    m_timers.clear();
    delete m_log;
    return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2MultiJetHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
// ----------------------------------------------------------------------
    
    // Time total TrigL2MultiJetHypo execution time.
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << "================= Executing TrigL2MultiJetHypo Hypo " << name() << endmsg;
    }
#endif
     
    if (m_timersvc) m_timers[0]->start();
    
    
    m_cutCounter = -1;
    m_nJets = -99;
    m_e.clear();
    m_et.clear();
    m_eta.clear();
    m_phi.clear();
    m_nLeadingTowers.clear();
    m_accepted_jets.clear();
    
    
    pass=false;
    
    // get the trigger element and extract the RoI information
    
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << "getting the vector of TrigT2Jet[s] from outputTE" << endmsg;
    }
#endif
    //
    std::vector<const TrigT2Jet*> vectorOfJets;
    //const TrigT2JetContainer* vectorOfJets(0);
    HLT::ErrorCode ec = getFeatures(outputTE, vectorOfJets, m_jetInputKey);
    
    if(ec!=HLT::OK) {
        (*m_log) << MSG::WARNING << " Failed to get the L2 Jets " << endmsg;
        if (m_timersvc) m_timers[0]->stop();
        return ec;
    }
    
    // The L2 jets should be sorted by ET
    //  now add the TrigPassBits for the jets to specify whether each jet 
    //  passes the hypo etc
    
    //TrigPassBits* bits = HLT::makeTrigPassBits( vectorOfJets ); // this needs a containter as augement not a vector
    
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << "Got " << vectorOfJets.size() << " jet(s)" << endmsg;
        (*m_log) << MSG::DEBUG << "Now performing cuts" << endmsg;
        (*m_log) << MSG::DEBUG << "This hypo assumes jets are sorted by descending ET" << endmsg;
     
    }
#endif
    
    //// now loop over jets
    //m_cutCounter = 0;
    unsigned int      multiplicity = 0;
    unsigned int  cut_multiplicity = 0;
    int jet_counter = -1;
    std::vector<const TrigT2Jet*>::const_iterator begin_jet, end_jet; // iterators for the produced jets
    begin_jet = vectorOfJets.begin();
    end_jet = vectorOfJets.end();
    
    for (; begin_jet != end_jet; ++begin_jet){
        jet_counter++;
        
        double jet_et = (*begin_jet)->et();
        double jet_eta = std::fabs((*begin_jet)->eta());
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            if (m_acceptAll) {
                (*m_log) << MSG::DEBUG << "Jet Et:    " << jet_et  << "\t accept all" << endmsg;
                (*m_log) << MSG::DEBUG << "Jet |eta|: " << jet_eta << "\t accept all" << endmsg;
            } else {
                (*m_log) << MSG::DEBUG << "Jet Et:    " << jet_et  << "\t Et cut: " << m_EtThresholds[cut_multiplicity] << endmsg;
                (*m_log) << MSG::DEBUG << "Jet |eta|: " << jet_eta << "\t eta min: " << m_etaMinCut << ", max: " << m_etaMaxCut << endmsg;            
            }
        }
#endif
        if ((m_acceptAll) || ((jet_et > m_EtThresholds[cut_multiplicity]) && ( (jet_eta >= m_etaMinCut) && (jet_eta < m_etaMaxCut) ))) {
#ifndef NDEBUG
            if((*m_log).level() <= MSG::DEBUG){
                (*m_log) << MSG::DEBUG << "   accepted"<<endmsg;
            }
#endif
            multiplicity++;
            m_accepted_jets.push_back(jet_counter);
            // flag all jets as having passed or not for single jet chains
            if ( m_multiplicitycut!=1  ) cut_multiplicity++;
            // flag this jet as having passed
            //HLT::markPassing( bits, (*begin_jet), vectorOfJets ); 
            //  careful here, the threshold vector m_EtThresholds only has m_multiplicitycut 
            //  thresholds, so mustn't try to test the et for more than that many jets or 
            //  the code will seg fault    
            if( multiplicity >= m_multiplicitycut && m_multiplicitycut>1 ) break;
        } else { 
#ifndef NDEBUG
            if((*m_log).level() <= MSG::DEBUG){
                (*m_log) << MSG::DEBUG << "   rejected"<<endmsg; 
            }
#endif
        }
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            (*m_log) << MSG::DEBUG << "Current multiplicity accepted: "<<multiplicity << endmsg;
            for(unsigned int i=0;i<m_accepted_jets.size();i++){
                (*m_log) << MSG::DEBUG << "   accepted jet: "<<m_accepted_jets[i]<< endmsg;
            }
          }
#endif
        
        
        }
    
    ////Et Cut
    if( multiplicity >= m_multiplicitycut || m_acceptAll) {
    pass = true;
    m_accepted_L2++;
    m_cutCounter=1;
    
    m_nJets = multiplicity;
    for(unsigned int i=0;i<m_accepted_jets.size();i++){
        m_e.push_back(vectorOfJets.at(m_accepted_jets[i])->e());
        m_et.push_back(vectorOfJets.at(m_accepted_jets[i])->et());
        m_eta.push_back(vectorOfJets.at(m_accepted_jets[i])->eta());
        m_phi.push_back(vectorOfJets.at(m_accepted_jets[i])->phi());
        m_nLeadingTowers.push_back(vectorOfJets.at(m_accepted_jets[i])->nLeadingCells());
    }
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << " Event accepted ! " << endmsg;
    }
#endif
    } else {
        m_rejected_L2++;
        m_cutCounter=0;
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            (*m_log) << MSG::DEBUG << " Event rejected !" << endmsg;
        }
#endif
    }
    
    
    // Time total TrigL2MultiJetHypo execution time.
    // -------------------------------------
    #ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG) {
        (*m_log) << MSG::DEBUG << "================= Finished TrigT2CaloJet Hypo " << name() << endmsg;
    }
    #endif
    if (m_timersvc) m_timers[0]->stop();
    
    return HLT::OK;

}
