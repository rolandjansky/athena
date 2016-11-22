/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigL2MultiJetAllTE.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigMultiJetHypo
//
// AUTHOR: Matthew Tamsett tamsett@cern.ch
//
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

//#include "TrigSteeringEvent/TriggerElement.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEvent/TrigT2JetContainer.h" // Not needed anymore

#include "TrigJetHypo/TrigL2MultiJetAllTE.h"

#include "CLHEP/Units/SystemOfUnits.h"


class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigL2MultiJetAllTE::TrigL2MultiJetAllTE(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::AllTEAlgo(name, pSvcLocator) {
    
    declareProperty("jetInputKey",      m_jetInputKey = "TrigT2CaloJet");
    declareProperty("Etcut_L2",         m_EtCut_L2 = 20*CLHEP::GeV, "cut value for L2 jet et"); // Default: 20 GeV
    declareProperty("Multiplicity_L2",  m_multiplicitycut = 1, "cut value for L2 jet multiplicity"); // Default: 1
    declareProperty("EtThresholds",     m_EtThresholdsInput ); // Default: 40 GeV
    declareProperty("etaMinCut",        m_etaMinCut    = 0.); // Default: 0
    declareProperty("etaMaxCut",        m_etaMaxCut    = 3.2); // Default: 3.2 - central jets
    declareProperty("n90Cut",           m_n90Cut = 0);  // cleaning cut
    declareProperty("etaMaxN90Cut",     m_etaMaxN90Cut = m_etaMaxCut);  // max eta for n90 cut
    
    
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
TrigL2MultiJetAllTE::~TrigL2MultiJetAllTE()
{ }

// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2MultiJetAllTE::hltInitialize()
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
        TrigTimer* tmp = m_timersvc->addItem("TrigL2MultiJetAllTE.TrigL2MultiJetAllTETot");
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
    (*m_log) << MSG::INFO << "\t n90 cut: " << m_n90Cut << endmsg; 
    
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
HLT::ErrorCode TrigL2MultiJetAllTE::hltFinalize(){
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
HLT::ErrorCode TrigL2MultiJetAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, unsigned int output) {
// ----------------------------------------------------------------------
    
    
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << "================= Executing TrigL2MultiJetAllTE Hypo " << name() << endmsg;
    }
#endif
    beforeExecMonitors().ignore();
    if (m_timersvc) m_timers[0]->start();    
    
    m_cutCounter = -1;
    m_nJets = -99;
    m_e.clear();
    m_et.clear();
    m_eta.clear();
    m_phi.clear();
    m_nLeadingTowers.clear();
    m_accepted_jets.clear();
    
    //bool pass=false;
    
    // get the trigger elements and extract the RoI information
    unsigned int      multiplicity = 0;
    unsigned int  cut_multiplicity = 0;
    int jet_counter = -1;
    std::vector<const TrigT2Jet*> vectorOfJets;
    
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << tes_in.size() << " input TEs found" << endmsg;        
    }
#endif

    for ( unsigned type = 0; type < tes_in.size(); ++ type ) {
        std::vector<HLT::TriggerElement*>& tes = tes_in.at(type);
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            (*m_log) << MSG::DEBUG << "  - TE[" << type << "]: " << tes.size() << " sub TEs found" << endmsg;        
        }
#endif
        
        for ( unsigned teIdx = 0; teIdx < tes.size(); ++teIdx ) {            
            HLT::ErrorCode ec = getFeatures(tes.at(teIdx), vectorOfJets, m_jetInputKey);
            //seednode = tes.at(teIdx);
            if(ec!=HLT::OK) {
                (*m_log) << MSG::WARNING << "Failed to get the input particles" << endmsg;
                return ec;
            }          
#ifndef NDEBUG
            if((*m_log).level() <= MSG::DEBUG){
                (*m_log) << MSG::DEBUG  << "  - A total of " << vectorOfJets.size()  << " jets found in all TEs so far" << endmsg;
            }
#endif
            
        } // end of loop on sub TEs
    } // end of loop on TEs
    
    // loop over the jets and make the trigger decision
    std::vector<const TrigT2Jet*>::const_iterator begin_jet, end_jet; // iterators for the produced jets
    begin_jet = vectorOfJets.begin();
    end_jet   = vectorOfJets.end();
    for (; begin_jet != end_jet; ++begin_jet){
        jet_counter++;
        double jet_et = (*begin_jet)->et();
        double jet_eta = std::fabs((*begin_jet)->eta());
        int jet_n90 = (*begin_jet)->nLeadingCells();
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            if (m_acceptAll) {
                (*m_log) << MSG::DEBUG << "Jet Et:    " << jet_et  << "\t accept all" << endmsg;
                (*m_log) << MSG::DEBUG << "Jet |eta|: " << jet_eta << "\t accept all" << endmsg;
                (*m_log) << MSG::DEBUG << "Jet n90: "   << jet_n90 << "\t accept all" << endmsg;
                
            } else {
                (*m_log) << MSG::DEBUG << "Jet Et:    " << jet_et  << "\t Et cut: "  << m_EtThresholds[cut_multiplicity] << endmsg;
                (*m_log) << MSG::DEBUG << "Jet |eta|: " << jet_eta << "\t eta min: " << m_etaMinCut << ", max: " << m_etaMaxCut << endmsg; 
                (*m_log) << MSG::DEBUG << "Jet n90: "   << jet_n90 << "\t n90 cut: " << m_n90Cut << ", eta max n90 cut: " << m_etaMaxN90Cut << endmsg;           
            }
        }
#endif
        // now check if this jet passed the required cuts
        if ((m_acceptAll) || ((jet_et > m_EtThresholds[cut_multiplicity]) && ( (jet_eta >= m_etaMinCut) && (jet_eta < m_etaMaxCut) ))) {
            if ( (m_n90Cut > 0) && (jet_n90 < m_n90Cut) && (jet_eta < m_etaMaxN90Cut) ) continue;
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
    } // end of loop on jets
    
    // look at accepted multiplicty and decide if we've passed
    if( multiplicity >= m_multiplicitycut || m_acceptAll) {
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            (*m_log) << MSG::DEBUG << " Event accepted ! " << endmsg;
        }
#endif
        //pass = true;
        m_accepted_L2++;
        
        // attach input TEs to output TE
        for ( unsigned type = 0; type < tes_in.size(); ++ type ) {
            std::vector<HLT::TriggerElement*>& tes = tes_in.at(type);
            for ( unsigned teIdx = 0; teIdx < tes.size(); ++teIdx ) {
                HLT::TriggerElement* out_te = config()->getNavigation()->addNode(tes.at(teIdx), output);
                out_te->setActiveState(true);
            }
        } // end of loop on TEs
        // monitor
        m_cutCounter=1;     
        m_nJets = multiplicity;
        for(unsigned int i=0;i<m_accepted_jets.size();i++){
            m_e.push_back(vectorOfJets.at(m_accepted_jets[i])->e());
            m_et.push_back(vectorOfJets.at(m_accepted_jets[i])->et());
            m_eta.push_back(vectorOfJets.at(m_accepted_jets[i])->eta());
            m_phi.push_back(vectorOfJets.at(m_accepted_jets[i])->phi());
            m_nLeadingTowers.push_back(vectorOfJets.at(m_accepted_jets[i])->nLeadingCells());
        }

    } else {
        m_rejected_L2++;
        m_cutCounter=0;
#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){
            (*m_log) << MSG::DEBUG << " Event rejected !" << endmsg;
        }
#endif
    } // end on check if passed
    

    // -------------------------------------
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG) {
        (*m_log) << MSG::DEBUG << "================= Finished TrigT2CaloJet Hypo " << name() << endmsg;
    }
#endif
    if (m_timersvc) m_timers[0]->stop();
    afterExecMonitors().ignore();
    return HLT::OK;

}


