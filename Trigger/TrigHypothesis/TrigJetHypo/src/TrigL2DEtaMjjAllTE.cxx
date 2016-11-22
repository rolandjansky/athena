/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigL2DEtaMjjAllTE.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigL2DEtaMjjAllTE
//
// AUTHOR: Justin Griffiths (griffith@cern.ch)
//
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

//#include "TrigSteeringEvent/TriggerElement.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEvent/TrigT2JetContainer.h" // Not needed anymore

#include "TrigJetHypo/TrigL2DEtaMjjAllTE.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>

class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigL2DEtaMjjAllTE::TrigL2DEtaMjjAllTE(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::AllTEAlgo(name, pSvcLocator),
  //m_storeGate(0),
  m_timersvc(0),
  m_log(0)
{
    
  declareProperty("jetInputKey",      m_jetInputKey = "TrigT2CaloJet");
  declareProperty("EtCut",            m_EtCut = 15*CLHEP::GeV, "cut value for L1.5 jet et"); // Default: 20 GeV
  declareProperty("etaMinCut",        m_etaMinCut    = -1.); // Default: 0
  declareProperty("etaMaxCut",        m_etaMaxCut    = 100); // Default: 3.2 - central jets
  declareProperty("dEtaCut",          m_dEtaCut      = 2.0);
  declareProperty("MjjCut",           m_MjjCut       = 0*CLHEP::GeV);
   
  declareProperty("doMonitoring_L2",  m_doMonitoring = false, "switch on/off monitoring" );
    
  declareMonitoredVariable("CutCounter",         m_cutCounter);
  declareMonitoredVariable("nJets",              m_nJets);
  declareMonitoredStdContainer("E",              m_e);
  declareMonitoredStdContainer("Et",             m_et);
  declareMonitoredStdContainer("Eta",            m_eta);
  declareMonitoredStdContainer("Phi",            m_phi);
  declareMonitoredVariable("dEta",            m_dEta);
  declareMonitoredVariable("mjj",                m_Mjj);
}
/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigL2DEtaMjjAllTE::~TrigL2DEtaMjjAllTE()
{ }

// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2DEtaMjjAllTE::hltInitialize()
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
    TrigTimer* tmp = m_timersvc->addItem("TrigL2DEtaMjjAllTE.TrigL2DEtaMjjAllTETot");
    m_timers.push_back(tmp);
  }
    
  (*m_log) << MSG::INFO << "\t et: " << m_EtCut << "\t eta cuts, min: " << m_etaMinCut << ", max: " << m_etaMaxCut << endmsg; 
    
  m_accepted_L2=0;
  m_rejected_L2=0;
  m_errors_L2=0;
    
  m_e.reserve(2);
  m_et.reserve(2);
  m_eta.reserve(2);
  m_phi.reserve(2);

  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2DEtaMjjAllTE::hltFinalize(){
  // ----------------------------------------------------------------------
    
  (*m_log) << MSG::INFO << "in finalize()" << endmsg;
  (*m_log) << MSG::INFO << "Events (Lvl2) accepted/rejected/errors:  "<< m_accepted_L2 <<" / "<<m_rejected_L2<< " / "<< m_errors_L2<< endmsg;
  m_e.clear();
  m_et.clear();
  m_eta.clear();
  m_phi.clear();
  m_timers.clear();
  delete m_log;
  m_log = 0;
  return HLT::OK;
}

/// A struct to use with stl sort to sort the jets by eta
struct sorter_eta{
  bool operator () (const TrigT2Jet *left, const TrigT2Jet *right) const {
    return left->eta() > right->eta();
  }
};


// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2DEtaMjjAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, unsigned int output) {
  // ----------------------------------------------------------------------
    

#ifndef NDEBUG
  if((*m_log).level() <= MSG::DEBUG){
    (*m_log) << MSG::DEBUG << "================= Executing TrigL2DEtaMjjAllTE Hypo " << name() << endmsg;
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
    
  //bool pass=false;
    
  // get the trigger elements and extract the RoI information
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

  if( vectorOfJets.size() < 2 ){
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
      (*m_log) << MSG::DEBUG  << "  - Event has " << vectorOfJets.size()  << " jets found in all TEs; < 2 Exiting" << endmsg;
    }
#endif
    m_rejected_L2++;
    return HLT::OK;
  }

    
  // loop over the jets and remove those jets not satisfying minimal pt,eta,..
  std::vector<const TrigT2Jet*> selectedVectorOfJets;    
  std::vector<const TrigT2Jet*>::const_iterator begin_jet, end_jet; // iterators for the produced jets
  begin_jet = vectorOfJets.begin();
  end_jet   = vectorOfJets.end();
  for (; begin_jet != end_jet; ++begin_jet){
    if( (*begin_jet)->et() < m_EtCut ) continue;
    if( fabs((*begin_jet)->eta()) < m_etaMinCut ) continue;
    if( fabs((*begin_jet)->eta()) > m_etaMaxCut ) continue;
    jet_counter++;
    selectedVectorOfJets.push_back( *begin_jet );
  }

  if( selectedVectorOfJets.size() < 2 ){
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
      (*m_log) << MSG::DEBUG  << "  - Event has " << selectedVectorOfJets.size()  << " jets found in all TEs passing kinematic requirements; < 2 Exiting" << endmsg;
    }
#endif
    m_rejected_L2++;
    return HLT::OK;
  }
    
  //sort jets by eta
  std::sort(selectedVectorOfJets.begin(), selectedVectorOfJets.end(), sorter_eta());
    
  const TrigT2Jet* jet1 = selectedVectorOfJets[0];
  const TrigT2Jet* jet2 = selectedVectorOfJets[selectedVectorOfJets.size()-1];

#ifndef NDEBUG
  if((*m_log).level() <= MSG::DEBUG){
    for(std::vector<const TrigT2Jet*>::const_iterator itr = selectedVectorOfJets.begin(); itr != selectedVectorOfJets.end(); ++itr){
      (*m_log) << MSG::DEBUG <<"jet et: "<< (*itr)->et() << " eta: "<< (*itr)->eta() << endmsg;
    }
  }
#endif

  m_dEta = fabs(jet1->eta()-jet2->eta());
  m_Mjj =  (jet1->hlv()+jet2->hlv()).m();

#ifndef NDEBUG
  if((*m_log).level() <= MSG::DEBUG){
    (*m_log) << MSG::DEBUG << "Jet 1 Eta :    " << jet1->eta()  << "\t Jet 2 Eta : " << jet2->eta() << "\t dEta " << m_dEta << "\t Mjj " << m_Mjj << endmsg;
  }
#endif
    
  // now check if these two jets passed the required cuts
  if ( m_dEta > m_dEtaCut && m_Mjj > m_MjjCut ) {
    //pass = true;
    m_accepted_L2++;
    m_cutCounter=1;
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
      (*m_log) << MSG::DEBUG << "   accepted"<<endmsg;
    }
#endif
    // attach input TEs to output TE
    for ( unsigned type = 0; type < tes_in.size(); ++ type ) {
      std::vector<HLT::TriggerElement*>& tes = tes_in.at(type);
      for ( unsigned teIdx = 0; teIdx < tes.size(); ++teIdx ) {
	HLT::TriggerElement* out_te = config()->getNavigation()->addNode(tes.at(teIdx), output);
	out_te->setActiveState(true);
      }
    } // end of loop on TEs
      // monitor
    m_nJets = selectedVectorOfJets.size();
    m_e.push_back(jet1->e());  m_e.push_back(jet2->e());
    m_et.push_back(jet1->et());  m_et.push_back(jet2->et());
    m_eta.push_back(jet1->eta());  m_eta.push_back(jet2->eta());
    m_phi.push_back(jet1->phi());  m_phi.push_back(jet2->phi());
      
  } else {
    m_rejected_L2++;
    m_cutCounter=0;
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
      (*m_log) << MSG::DEBUG << " Event rejected !" << endmsg;
    }
#endif
  }
    
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


