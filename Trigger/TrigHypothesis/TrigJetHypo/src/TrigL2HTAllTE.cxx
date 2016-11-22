/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigL2HTAllTE.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigMultiJetHypo
//
// AUTHOR:   Matthew Tamsett, Marco Agustoni
//
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

//#include "TrigSteeringEvent/TriggerElement.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEvent/TrigT2JetContainer.h" // Not needed anymore

#include "TrigJetHypo/TrigL2HTAllTE.h"

#include "CLHEP/Units/SystemOfUnits.h"

/// to add TrigPassBits 22/11/2011
//#include "TrigSteeringEvent/TrigPassBits.h"
/// adding TrigPassFlags for isEM bits 22/11/2011
//#include "TrigSteeringEvent/TrigPassFlags.h"


class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigL2HTAllTE::TrigL2HTAllTE(const std::string& name, ISvcLocator* pSvcLocator):
    HLT::AllTEAlgo(name, pSvcLocator) {

    declareProperty("jetInputKey",      m_jetInputKey = "TrigT2CaloJet");
    declareProperty("HTcut",           m_HTCut        = 350*CLHEP::GeV    ); // Default: 350 GeV
    declareProperty("Etcut",           m_EtCut        = 30*CLHEP::GeV     ); // Default: 30 GeV
    declareProperty("etaMincut",       m_etaMinCut    = 0.         ); // Default: 0
    declareProperty("etaMaxcut",       m_etaMaxCut    = 3.2        ); // Default: 3.2 - central jets
    
    declareProperty("doMonitoring_L2", m_doMonitoring = true, "switch on/off monitoring" );
    declareProperty("AcceptAll",       m_acceptAll    = false                             );
    //  declareProperty("histoPath",       m_path         = "/stat/Monitoring/EventFilter"    );
    
    declareMonitoredVariable("CutCounter",         m_cutCounter    );
    declareMonitoredVariable("n_tot_Jets",         m_n_tot_Jets    );
    declareMonitoredVariable("n_acc_Jets",         m_n_acc_Jets    );
    declareMonitoredVariable("n_rej_Jets",         m_n_rej_Jets    );
    declareMonitoredVariable("HT",                 m_HT            );
    declareMonitoredStdContainer("E",              m_e             ); 
    declareMonitoredStdContainer("Et_acc",         m_et_acc        );
    declareMonitoredStdContainer("Eta_acc",        m_eta_acc       );
    declareMonitoredStdContainer("Phi_acc",        m_phi_acc       );
    declareMonitoredStdContainer("Et_rej",         m_et_rej        );
    declareMonitoredStdContainer("Eta_rej",        m_eta_rej       );
    declareMonitoredStdContainer("Phi_rej",        m_phi_rej       );
    declareMonitoredStdContainer("nLeadingTowers", m_nLeadingTowers);
  
  
}
/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigL2HTAllTE::~TrigL2HTAllTE()
{ }

// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2HTAllTE::hltInitialize()
// ----------------------------------------------------------------------
{
    m_log = new MsgStream(msgSvc(), name());
    (*m_log) << MSG::INFO << "in initialize(): " << name() << endmsg;
    (*m_log) << MSG::INFO << "Eta min Cut: " << m_etaMinCut << endmsg;
    (*m_log) << MSG::INFO << "Eta max Cut: " << m_etaMaxCut << endmsg;
    (*m_log) << MSG::INFO << "ET Cut: " << m_EtCut << endmsg;
    (*m_log) << MSG::INFO << "HT Cut: " << m_HTCut << endmsg;
    
    // Initialize timing service
    // Use this tool to measure partial timings inside the algorithm, total execution time is monitored by the steering
    //------------------------------
    /*  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
        (*m_log) << MSG::WARNING << name()
        << ": Unable to locate TrigTimer Service" << endmsg;
    }
    if (m_timersvc) {
        TrigTimer* tmp = m_timersvc->addItem("TrigL2HTAllTE.TrigL2HTAllTETot");
        m_timers.push_back(tmp);
    }
    */
    m_accepted_L2 = 0; //number of events accepted
    m_rejected_L2 = 0; //number of events rejected
    m_errors_L2   = 0; //errors in accepting or not events at L2 with HT cut 
    
    m_e.reserve(100);
    m_et_acc.reserve(100);
    m_eta_acc.reserve(100);
    m_phi_acc.reserve(100);
    m_et_rej.reserve(100);
    m_eta_rej.reserve(100);
    m_phi_rej.reserve(100);
    m_nLeadingTowers.reserve(100);
    m_accepted_jets.reserve(100);
    
    
    return HLT::OK;
} // end initialize

// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2HTAllTE::hltFinalize()
// ----------------------------------------------------------------------
{
  (*m_log) << MSG::INFO << "in finalize()" << endmsg;
  (*m_log) << MSG::INFO << "Events (Lvl2) accepted/rejected/errors:  "<< m_accepted_L2 <<" / "<<m_rejected_L2<< " / "<< m_errors_L2<< endmsg;
  m_e.clear();
  m_et_acc.clear();
  m_eta_acc.clear();
  m_phi_acc.clear();
  m_et_rej.clear();
  m_eta_rej.clear();
  m_phi_rej.clear();
  m_nLeadingTowers.clear();
  m_accepted_jets.clear();
//  m_timers.clear();
  delete m_log;
  return HLT::OK;
} // end finalize

// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2HTAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, unsigned int output) 
// ----------------------------------------------------------------------
{

  // Time total TrigL2HTAllTE execution time.
#ifndef NDEBUG
  if((*m_log).level() <= MSG::DEBUG){
     (*m_log) << MSG::DEBUG << "================= Executing TrigL2HTAllTE Hypo " << name() << endmsg;
  }
#endif
  beforeExecMonitors().ignore();
     
//  if (m_timersvc) m_timers[0]->start();
  
  m_HT           = 0.; // HT variable: sum of etjets
  m_cutCounter   = -1; // flag for events rejection
  m_n_tot_Jets   = 0;  // counts the total number of jets per event 
  m_n_acc_Jets   = 0;  // counts the number of accepted jets per event 
  m_n_rej_Jets   = 0;  // counts the number of rejected jets per event 
  m_e.clear();
  m_et_acc.clear();
  m_eta_acc.clear();
  m_phi_acc.clear();
  m_et_rej.clear();
  m_eta_rej.clear();
  m_phi_rej.clear();
  m_nLeadingTowers.clear();
  m_accepted_jets.clear();
  

#ifndef NDEBUG  
  bool pass=false; // it comunicates if jets pass the trigger or not
#endif
  
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
  
  
  // generate TrigPassBits mask to flag which egamma objects pass hypo cuts
  //     TrigPassBits* passBits = HLT::makeTrigPassBits(vectorOfJets);


//*************** now loop over jets ***************

  unsigned int HT_counter = 0;  // counts how many jets fill HT
  int jet_counter         = -1; // indices of jets


  std::vector<const TrigT2Jet*>::const_iterator begin_jet, end_jet; // iterators for the produced jets
  begin_jet = vectorOfJets.begin();
  end_jet = vectorOfJets.end();

    for (; begin_jet != end_jet; ++begin_jet){ // begin jet loop
         jet_counter++;
	 m_n_tot_Jets++;

         double etjet = (*begin_jet)->et();
         double jeteta = (*begin_jet)->eta();
  
//fill HT requiring Et and eta cuts
    	 if(etjet >= m_EtCut && std::fabs(jeteta) >= m_etaMinCut && std::fabs(jeteta) <= m_etaMaxCut){
         	m_HT += etjet;
	 	HT_counter++; 
		m_accepted_jets.push_back(jet_counter);  
	 
	   } // end HT filling
	
#ifndef NDEBUG
         if((*m_log).level() <= MSG::DEBUG){
             double jetphi = (*begin_jet)->phi();
            (*m_log) << MSG::DEBUG << "jet_counter " << jet_counter << endmsg;
            (*m_log) << MSG::DEBUG << "Jet Et " << etjet << endmsg;
	    (*m_log) << MSG::DEBUG << "jeteta " << jeteta << endmsg;
	    (*m_log) << MSG::DEBUG << "jetphi " << jetphi << endmsg;
            (*m_log) << MSG::DEBUG << "HT = " << m_HT << endmsg;
            (*m_log) << MSG::DEBUG << "jets which fill HT: " << HT_counter << endmsg;
            (*m_log) << MSG::DEBUG << " # m_accepted_jets " << m_accepted_jets.size() << endmsg;
           }        
#endif
    
  }// end jet loop

#ifndef NDEBUG
  if((*m_log).level() <= MSG::DEBUG){
     (*m_log) << MSG::DEBUG << "Current HT accepted: "<< m_HT << endmsg;
      for(unsigned int i=0;i<m_accepted_jets.size();i++){
          (*m_log) << MSG::DEBUG << " indices of jets which are accepted "<<m_accepted_jets[i]<< endmsg;
         }
     }
#endif


//*************** HT Cut ***************

  if( m_HT >= m_HTCut || m_acceptAll) {
#ifndef NDEBUG
      pass = true;
#endif
      m_accepted_L2++;
      m_cutCounter=1;
      // attach input TEs to output TE
      for ( unsigned type = 0; type < tes_in.size(); ++ type ) {
          std::vector<HLT::TriggerElement*>& tes = tes_in.at(type);
          for ( unsigned teIdx = 0; teIdx < tes.size(); ++teIdx ) {
              HLT::TriggerElement* out_te = config()->getNavigation()->addNode(tes.at(teIdx), output);
              out_te->setActiveState(true);
          }
      } // end of loop on TEs

      m_n_acc_Jets = HT_counter; // # of jets which fill HT

      for(unsigned int i=0;i<m_accepted_jets.size();i++){
          m_e.push_back(vectorOfJets.at(m_accepted_jets[i])->e());
          m_et_acc.push_back(vectorOfJets.at(m_accepted_jets[i])->et());
          m_eta_acc.push_back(vectorOfJets.at(m_accepted_jets[i])->eta());
          m_phi_acc.push_back(vectorOfJets.at(m_accepted_jets[i])->phi());
          m_nLeadingTowers.push_back(vectorOfJets.at(m_accepted_jets[i])->nLeadingCells());
         }
#ifndef NDEBUG
      if((*m_log).level() <= MSG::DEBUG){
         (*m_log) << MSG::DEBUG << " Event accepted: " << pass << endmsg;
        }
#endif
     } // accepted events, they satisfy HT cut
else {
      m_rejected_L2++;
      m_n_rej_Jets++;
      m_cutCounter=0;

      for(unsigned int i=0;i<m_accepted_jets.size();i++){
          m_et_rej.push_back(vectorOfJets.at(m_accepted_jets[i])->et());
          m_eta_rej.push_back(vectorOfJets.at(m_accepted_jets[i])->eta());
          m_phi_rej.push_back(vectorOfJets.at(m_accepted_jets[i])->phi());
         }
     } // rejected events, they do not satisfy HT cut
  
  // Time total TrigL2HTAllTE execution time.
  // -------------------------------------
#ifndef NDEBUG

  if((*m_log).level() <= MSG::DEBUG) {
    (*m_log) << MSG::DEBUG << "================= Finished TrigL2HTAllTE " << name() << endmsg;
    (*m_log) << MSG::DEBUG << " Trigger Result =" << pass <<endmsg;
    (*m_log) << MSG::DEBUG << " Accept all =" << m_acceptAll <<endmsg;
  }
#endif
//  if (m_timersvc) m_timers[0]->stop();
  afterExecMonitors().ignore();
  return HLT::OK;

} // end execute
