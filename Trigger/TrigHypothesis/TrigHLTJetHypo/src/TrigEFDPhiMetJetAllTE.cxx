/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEFDPhiMetJetAllTE.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// ********************************************************************

// #include "GaudiKernel/StatusCode.h"

#include "TrigHLTJetHypo/TrigEFDPhiMetJetAllTE.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

#include "xAODTrigger/TrigPassBits.h"
#include <math.h>

#include "GaudiKernel/SystemOfUnits.h"

TrigEFDPhiMetJetAllTE::TrigEFDPhiMetJetAllTE(const std::string & name, ISvcLocator* pSvcLocator)
 : HLT::AllTEAlgo(name, pSvcLocator)
//,
//   m_executedEvent_EFDPhiMetJet(false)
{
  declareProperty("MinDPhiCut",   m_MinDPhiCut  = 0.3    );
  declareProperty("MaxDPhiJets",  m_MaxDPhiJets = 3      ); 
  declareProperty("MinJetEt",     m_MinJetEt    = 20*Gaudi::Units::GeV  ); 
  declareProperty("MinJetEta",    m_MinJetEta   = 0   );   
  declareProperty("MaxJetEta",    m_MaxJetEta   = 3.2 ); 
  
  declareMonitoredVariable("NJets",             m_n_jets         ); 
  declareMonitoredVariable("jet1et",            m_jet1et         ); 
  declareMonitoredVariable("jet1eta",           m_jet1eta        ); 
  declareMonitoredVariable("jet1phi",           m_jet1phi        );
  declareMonitoredVariable("jet_met_phi1",      m_jet_met_phi1   );
  declareMonitoredVariable("jet2et",            m_jet2et         ); 
  declareMonitoredVariable("jet2eta",           m_jet2eta        ); 
  declareMonitoredVariable("jet2phi",           m_jet2phi        );
  declareMonitoredVariable("jet_met_phi2",      m_jet_met_phi2   );

  declareMonitoredVariable("NJets_Pass",        m_n_jets_Pass    );
  declareMonitoredVariable("jet1et_Pass",       m_jet1et_Pass    );
  declareMonitoredVariable("jet1eta_Pass",      m_jet1eta_Pass   );
  declareMonitoredVariable("jet1phi_Pass",      m_jet1phi_Pass   );
  declareMonitoredVariable("jet_met_phi1_Pass", m_jet_met_phi1_Pass );
  declareMonitoredVariable("jet2et_Pass",       m_jet2et_Pass    );
  declareMonitoredVariable("jet2eta_Pass",      m_jet2eta_Pass   );
  declareMonitoredVariable("jet2phi_Pass",      m_jet2phi_Pass   );
  declareMonitoredVariable("jet_met_phi2_Pass", m_jet_met_phi2_Pass );
}

HLT::ErrorCode TrigEFDPhiMetJetAllTE::hltInitialize()
{
  ATH_MSG_DEBUG("Initialization. The correct configuration of this algorithm "
                << "requires jets ordered in increasing energy");
  
  
  m_acceptedEvts = 0;
  m_rejectedEvts = 0;
  m_errorEvts    = 0;
  
  return HLT::OK;
}

struct DescendingEt:std::binary_function<const xAOD::Jet*,
                                         const xAOD::Jet*,
                                         bool> {
  bool operator()(const xAOD::Jet* l, const xAOD::Jet* r)  const {
    return l->p4().Et() > r->p4().Et();
  }
};

TrigEFDPhiMetJetAllTE::~TrigEFDPhiMetJetAllTE(){
}

HLT::ErrorCode TrigEFDPhiMetJetAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
				     unsigned int output)
{
  
  ATH_MSG_DEBUG("***  Executing this TrigJetHypo : " << name());
  
  if (tes_in.size() < 1) {
    ATH_MSG_WARNING("No TriggerElements provided -> do nothing");
    //    m_executedEvent_EFDPhiMetJet = true;
    m_errorEvts+=1;
    m_rejectedEvts+=1;

    // since no TE are found while there should be something 
    // (why would the chain start otherwise?)
    return HLT::MISSING_FEATURE;
  }
  
  else if (tes_in.size() < 2) {
    ATH_MSG_WARNING("Less than 2 TriggerElements provided -> do nothing");
    //    m_executedEvent_EFDPhiMetJet = true;
    m_errorEvts+=1;
    m_rejectedEvts+=1;
    
    // since no TE are found while there should be something 
    // (why would the chain start otherwise?)
    return HLT::MISSING_FEATURE;
  }
  
  if(tes_in[0].size() == 0) {
    ATH_MSG_WARNING("No MET TE found");
    
    // put here not OK but something that is not aborting further execution
    return HLT::MISSING_FEATURE;
  }
  
  if(tes_in[1].size() == 0) {
    ATH_MSG_WARNING("No jet TE found");

    // put here not OK but something that is not aborting further execution
    return HLT::MISSING_FEATURE;
  }
  
  beforeExecMonitors().ignore();
  
  
  HLT::TEVec allTEs;
  // Retrieve MET from tes_in[0]

  std::vector<const xAOD::TrigMissingET*> vectorMissingET;
  HLT::ErrorCode statMET = getFeatures(tes_in[0].front(), vectorMissingET );
  
  if(statMET != HLT::OK) {
    ATH_MSG_WARNING("Failed to get vectorMissingETs");
    return HLT::OK;
  }
  
  if(vectorMissingET.size()==0){
    ATH_MSG_WARNING("Failed to get vectorMissingETs");
    return HLT::MISSING_FEATURE;
  }
  
  if ( (tes_in.size()>0) && (tes_in[0].size()>0) ) {
    allTEs.push_back( tes_in[0][0] );
  }

  const xAOD::TrigMissingET* efmet = vectorMissingET.front();
  float metphi = atan2f(efmet->ey(),efmet->ex());  

  // Retrieve jets from tes_in[1]

  const xAOD::JetContainer* outJets(0);
  HLT::ErrorCode statJets = getFeature(tes_in[1].front(), outJets);


  //  now add the TrigPassBits for the jets to specify whether each jet 
  //  passes the hypo etc
  // TrigPassBits* bits = HLT::makeTrigPassBits( outJets );
  std::unique_ptr<xAOD::TrigPassBits> xBits = 
    xAOD::makeTrigPassBits<xAOD::JetContainer>(outJets);


  if(statJets!=HLT::OK) {
    ATH_MSG_WARNING("Failed to get JetCollections");
    return HLT::OK;
  }

  if( outJets == 0 ){
    ATH_MSG_DEBUG("Got no JetCollections associated to the TE!");
    m_errorEvts++;
    return HLT::MISSING_FEATURE;
  }

  if ( (tes_in.size()>0) && (tes_in[1].size()>0) ){
    allTEs.push_back( tes_in[1][0] );
  }
  
  std::vector<const xAOD::Jet*> theJets(outJets->begin(), outJets->end());

  //check size of JetCollection
  if( theJets.size() == 0 ){
    ATH_MSG_DEBUG("Size of JetCollection is 0!");
    m_errorEvts++;
    //    if (m_timersvc) m_timers[0]->stop();
    return HLT::OK;
  }
  std::sort (theJets.begin(), theJets.end(), DescendingEt());

  //const double pi = 3.141592; //replaced with M_PI
  unsigned int jet_counter = 0;
  m_n_jets_Pass = 0;

  bool passDPhiCut = true;

  m_jet_met_phi1 = -999;
  m_jet1et       = -999;
  m_jet1phi      = -999;
  m_jet1eta      = -999;
  m_jet_met_phi2 = -999;
  m_jet2et       = -999;
  m_jet2phi      = -999;
  m_jet2eta      = -999;

  m_jet_met_phi1_Pass = -999;
  m_jet1et_Pass       = -999;
  m_jet1phi_Pass      = -999;
  m_jet1eta_Pass      = -999;
  m_jet_met_phi2_Pass = -999;
  m_jet2et_Pass       = -999;
  m_jet2phi_Pass      = -999;
  m_jet2eta_Pass      = -999;

  for (const xAOD::Jet* aJet : theJets) {

    m_jetet = aJet->p4().Et();
    m_jetphi = aJet->p4().Phi();
    m_jeteta = std::fabs(aJet->eta());

    double delta_phi = fabs(m_jetphi - metphi);                 
    if (delta_phi>M_PI)  delta_phi = 2*M_PI - delta_phi;

    //for monitoring before checking if pass -->
    if (jet_counter==0) {
      m_jet_met_phi1 = delta_phi;
      m_jet1et  = m_jetet;
      m_jet1phi = m_jetphi;
      m_jet1eta = m_jeteta;
    }
    else if (jet_counter ==1) {
      m_jet_met_phi2 = delta_phi;
      m_jet2et  = m_jetet;
      m_jet2phi = m_jetphi;
      m_jet2eta = m_jeteta;
    }
    //for monitoring before checking if pass --<


    if (jet_counter < m_MaxDPhiJets ) {
      if ( m_jetet >  m_MinJetEt && 
           m_jeteta >= m_MinJetEta && 
           m_jeteta < m_MaxJetEta && 
           delta_phi < m_MinDPhiCut ) {
        passDPhiCut = false;
      } else { // some monitoring -->
        
        
        ATH_MSG_DEBUG("Jet energy and phi: " 
                      << m_jetet 
                      << m_jetphi 
                      << " phi cut: " 
                      << m_MinDPhiCut);
        if (jet_counter==0) {
          m_jet_met_phi1_Pass = delta_phi;
          m_jet1et_Pass  = m_jetet;
          m_jet1phi_Pass = m_jetphi;
          m_jet1eta_Pass = m_jeteta;
        } else if (jet_counter ==1) {
          m_jet_met_phi2_Pass = delta_phi;
          m_jet2et_Pass  = m_jetet;
          m_jet2phi_Pass = m_jetphi;
          m_jet2eta_Pass = m_jeteta;
        }
        m_n_jets_Pass++;

        // flag these jets as having passed
        // HLT::markPassing( bits, aJet, outJets ); 
        xBits->markPassing(aJet, outJets, true); 
        
      } // monitoring --<
    }


    jet_counter++;

    m_n_jets = jet_counter;
    
    

    if (jet_counter >= m_MaxDPhiJets) break;
  }
  

  HLT::TriggerElement* out_te = 
    config()->getNavigation()->addNode(allTEs, output);    
  out_te->setActiveState(false);	
  
  if (passDPhiCut==true){
    //passCentralCut = true;

    out_te->setActiveState(true);
    m_acceptedEvts+=1;
  }
  else m_rejectedEvts+=1;


  int nJet=0;
  for (const xAOD::Jet* testJet : theJets) {
    ATH_MSG_DEBUG("Jet "
                  << (nJet+1) 
                  << " | TrigPassBit: " 
                  << xBits->isPassing(testJet, outJets ) 
                  << " Et: "<< testJet->p4().Et() 
                  << " Eta: "<< testJet->eta());
    nJet++;
  }
  


  auto sc = attachFeature(out_te, xBits.release(), "passbits");
  if(sc != HLT::OK) {
    ATH_MSG_ERROR("Could not store TrigPassBits");
    return sc;
  }


  
  //  m_executedEvent_EFDPhiMetJet = true;
  afterExecMonitors().ignore();
  return HLT::OK;
  
}


HLT::ErrorCode TrigEFDPhiMetJetAllTE::hltFinalize(){
  ATH_MSG_VERBOSE("in finalize()" 
               << "Events accepted/rejected/errors:  "
               << m_acceptedEvts 
               <<" / "
               << m_rejectedEvts 
               << " / "
               << m_errorEvts);
  return HLT::OK;
}

