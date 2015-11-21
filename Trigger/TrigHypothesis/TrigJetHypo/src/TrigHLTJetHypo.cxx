/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTJetHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:
//
// ********************************************************************
//

#include <algorithm>
#include <sstream>
// #include <cmath>
#include <stdexcept>
#include <memory>
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"

#include "TrigSteeringEvent/Enums.h"
#include "TrigJetHypo/TrigHLTJetHypo.h"
#include "TrigJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoUtils.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

#include "JetUtils/JetCaloQualityUtils.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "TrigSteeringEvent/TrigPassBits.h"
#include "./TrigHLTJetHypoHelpers/matcherFactory.h"
#include "./TrigHLTJetHypoHelpers/cleanerFactory.h"


TrigHLTJetHypo::TrigHLTJetHypo(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {

  
  m_accepted=0;
  m_rejected=0;
  m_errors=0;
  m_timersvc = nullptr;

  declareProperty("doMonitoring", m_doMonitoring = false );
  declareProperty("AcceptAll",      m_acceptAll=false);

  // declareProperty("EtThresholds",   m_EtThresholdsInput ); // Default: 40 GeV
  declareProperty("EtThresholds",   m_EtThresholds ); // Default: 40 GeV
  declareProperty("eta_mins",   m_etaMins);
  declareProperty("eta_maxs",   m_etaMaxs);
 
  // cleaning
  declareProperty("cleaningAlg", m_cleaningAlg = "noCleaning");
  declareProperty("matchingAlg", m_matchingAlg = "maximumBipartite");
  //basic cleaning  
  declareProperty("n90CleaningThreshold", m_n90Threshold = 2 );
  declareProperty("presamplerCleaningThreshold", m_presamplerThreshold = 0.9 );
  declareProperty("negativeECleaningThreshold", m_negativeEThreshold = -60e3 ); // 60 GeV
  declareProperty("qmeanCleaningThreshold", m_qmeanThreshold = 0.8 );
  declareProperty("HECQCleaningThreshold", m_hecQThreshold = 0.5 );
  declareProperty("HECfCleaningThreshold", m_hecFThreshold = 0.5 );
  declareProperty("LArQCleaningThreshold", m_larQThreshold = 0.8 );
  declareProperty("EMfCleaningThreshold", m_emFThreshold = 0.95 );
  //loose cleaning
  declareProperty("fracSamplingMaxLooseThreshold", m_fSampMaxLooseThreshold = 0.8 );
  declareProperty("etaLooseThreshold", m_etaLooseThreshold = 2.0 );
  declareProperty("EMfLowLooseThreshold", m_emfLowLooseThreshold = 0.10 );
  declareProperty("EMfHighLooseThreshold", m_emfHighLooseThreshold = 0.99 );
  declareProperty("HECfLooseThreshold", m_hecfLooseThreshold = 0.85 );
  //tight cleaning
  declareProperty("fracSamplingMaxTightThreshold", m_fSampMaxTightThreshold = 0.8 );
  declareProperty("etaTightThreshold", m_etaTightThreshold = 2.0 );
  declareProperty("EMfLowTightThreshold", m_emfLowTightThreshold = 0.10 );
  declareProperty("EMfHighTightThreshold", m_emfHighTightThreshold = 0.99 );
  declareProperty("HECfTightThreshold", m_hecfTightThreshold = 0.85 );
  //long-lived particle cleaning
  declareProperty("fracSamplingMaxLlpThreshold", m_fSampMaxLlpThreshold = 0.85 );
  declareProperty("negativeELlpThreshold", m_negELlpThreshold = -10e3 ); // 10 GeV
  declareProperty("HECfLlpThreshold", m_hecfLlpThreshold = 0.5 );
  declareProperty("HECQLlpThreshold", m_hecfLlpThreshold = 0.5 );
  declareProperty("AverageLArQFLlpThreshold", m_avLarQFLlpThreshold = 0.8*65535 );


  declareMonitoredVariable("CutCounter", m_cutCounter);

  // Monitored variables...
  declareMonitoredVariable("NJet", m_njet);
  declareMonitoredVariable("Et", m_et);
  declareMonitoredVariable("Eta", m_eta);
  declareMonitoredVariable("Phi", m_phi);
}

TrigHLTJetHypo::~TrigHLTJetHypo(){
}

HLT::ErrorCode TrigHLTJetHypo::hltInitialize()
{
  msg() << MSG::INFO << "in initialize()" << endreq;

  // Initialize timing service
  //------------------------------
  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
    msg() << MSG::WARNING << name()
	  << ": Unable to locate TrigTimer Service" << endreq;
  }

  if (m_timersvc) {
    TrigTimer* tmp = m_timersvc->addItem("TrigHLTJetHypo.TrigHLTJetHypoTot");
    m_timers.push_back(tmp);
  }

  if (m_EtThresholds.size() != m_etaMins.size() or
      m_EtThresholds.size() != m_etaMaxs.size()){

    msg() << MSG::ERROR
          << name()
          << ": mismatch between number of thresholds and eta min, max boundaries: "
          << m_EtThresholds.size() << " "
          << m_etaMins.size() << " "
          << m_etaMaxs.size() << " "
          << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB,HLT::Reason::BAD_JOB_SETUP);
  }   
  
  for (std::size_t i = 0; i != m_EtThresholds.size(); ++i){
    m_conditions.push_back(Condition(m_etaMins[i],
                                     m_etaMaxs[i],
                                     m_EtThresholds[i]));
  }

  std::sort(m_conditions.begin(), m_conditions.end(), ConditionsSorter());

  /*
  std::string msg = 'JetHypo conditions (threshold, eta min, eta max): \n';
  for(auto c : m_conditions){
    msg += c.threshold() + " " + c.etaMin() + " " + c.etaMax() + '\n';
  }
  ATH_MSG_INFO(msg);
  */

  // Later code assumes conditions vector is not empty.
  if (m_conditions.empty()){
    ATH_MSG_ERROR("TrigHLTJetHypo bad configuration, no conditions given");
    return HLT::ERROR;
  }
    
  ATH_MSG_INFO("TrigHLTJetHypo conditions: "<< m_conditions.size());
  for(auto c: m_conditions){ATH_MSG_INFO(c);}
  ATH_MSG_INFO("TrigHLTJetHypo cleaner: "<< m_cleaningAlg);
  ATH_MSG_INFO("TrigHLTJetHypo matcher: "<< m_matchingAlg);


  return HLT::OK;
  
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigHLTJetHypo::hltFinalize(){
  // ----------------------------------------------------------------------

  msg() << MSG::INFO << "in finalize()" << endreq;
  msg() << MSG::INFO << "Events accepted/rejected/errors:  "<< m_accepted <<" / "<<m_rejected<< " / "<< m_errors<< endreq;
  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigHLTJetHypo::hltExecute(const HLT::TriggerElement* outputTE,
                                         bool& pass) {

  ATH_MSG_DEBUG("Executing " << name() << "...");
  ATH_MSG_DEBUG("outputTE->getId(): " << outputTE->getId());

  pass=false;

  resetCounters();
  const xAOD::JetContainer* outJets(0);
  auto status = getFeature(outputTE, outJets);
  if (status != HLT::OK){ATH_MSG_ERROR("Jet retrieval failed"); return status;}

  status = checkJets(outJets) != HLT::OK;
  if (status != HLT::OK){ATH_MSG_ERROR("Jet checks failed"); return status;}
    

  /* copy the jets to a non-constant container */
  std::vector<const xAOD::Jet*> theJets(outJets->begin(), outJets->end());

  /* set up jet cleaning and hypothesis algorithm according to the user
     set flags */

  CleanerMatcher cleanerMatcher(std::shared_ptr<ICleaner>{},
                                std::shared_ptr<IMatcher>{});

  try{
    cleanerMatcher = getCleanerMatcher();
  } catch (std::invalid_argument& e){
    ATH_MSG_ERROR("Exception raised creating cleanerMatcher: " << e.what());
    return HLT::ERROR;
  }

   /* apply cleaning and hypotheis alg */
  ATH_MSG_DEBUG("cleaning and Matching start... " << name() << "...");
  try{
    cleanerMatcher.process(theJets.begin(), theJets.end());
  } catch(std::exception& e){
    ATH_MSG_ERROR("Exception raised by cleanerMatcher: " << e.what());
    return HLT::ERROR;
  }

  ATH_MSG_DEBUG("cleaning and matching done... " << name() << "...");

  pass = !theJets.empty() && (cleanerMatcher.pass() || m_acceptAll);
  
  publishResult(cleanerMatcher, pass, outJets);
  status = markAndStorePassingJets(cleanerMatcher, outJets, outputTE);
  if (status != HLT::OK){ATH_MSG_ERROR("Trig bits storage failed"); 
    return status;}
  

  return HLT::OK;
}

HLT::ErrorCode TrigHLTJetHypo::checkJets(const xAOD::JetContainer* outJets){

   ATH_MSG_DEBUG("Obtained JetContainer");

   m_cutCounter = 0;

   if(outJets == 0){
     ATH_MSG_WARNING("Jet collection pointer is 0");
     ++m_errors;
     if (m_acceptAll){m_cutCounter = 1;}
     return HLT::ERROR;
   } else {
     ATH_MSG_DEBUG("No of jets in: " << outJets->size());
     return HLT::OK;
   }
 }


 void 
 TrigHLTJetHypo::publishResult(const CleanerMatcher & cm, bool pass, const xAOD::JetContainer*& outJets){
   ATH_MSG_DEBUG("Publishing " << name() << "...");
   auto passedJetIters = cm.passedJetIters();
   auto failedJetIters = cm.failedJetIters();

   ATH_MSG_DEBUG("Publishing.monitoring " << name() << "...");
   /* The jets used to satisfy the hypo conditions may not contain the leading jet,
      so find this now */

   if(pass){
     auto leading_jet = std::max_element(outJets->begin(),
                                 outJets->end(),
                                 [](const xAOD::Jet* j0,
                                    const xAOD::Jet* j1){
                                   return j0->p4().Et() < j1->p4().Et();});

     monitorLeadingJet(*leading_jet);
   }

   int multiplicity = passedJetIters.second - passedJetIters.first;
   writeDebug(pass, multiplicity, passedJetIters, failedJetIters);
   bumpCounters(pass, multiplicity);
 }


void TrigHLTJetHypo::bumpCounters(bool pass, int multiplicity){
   if (pass){
     ++m_accepted;
     m_cutCounter=1;
     m_njet = multiplicity;
   } else {
     ++m_rejected;
   }
 }

void TrigHLTJetHypo::monitorLeadingJet(const xAOD::Jet* jet){
   ATH_MSG_DEBUG("monitoringLeadingJet " << name() << "...");
   m_et = jet->p4().Et();
   m_eta = jet->eta();
   m_phi = jet->phi();
   ATH_MSG_DEBUG("monitoringLeadingJet done " << name() << "...");
 }

void 
 TrigHLTJetHypo::writeDebug(bool pass,
                            int multiplicity,
                            const std::pair<JetCIter, JetCIter>& passedJetIters,
                            const std::pair<JetCIter, JetCIter>& failedJetIters
                            ) const{
    ATH_MSG_DEBUG("Writing debug " << name() << "...");
   // Et Cut
    if(pass){ATH_MSG_DEBUG("Event accepted");}
    else { ATH_MSG_DEBUG("Event rejected");}


   ATH_MSG_DEBUG("found multiplicity: "
                 << multiplicity
                 << "\t multiplicity cut: "
                 << m_conditions.size()
                 << ("passed Jets:"));

   for (JetCIter j = passedJetIters.first; j != passedJetIters.second; ++j) {
     ATH_MSG_DEBUG((*j)->p4().Et() << " " << (*j) -> eta());
   }

   ATH_MSG_DEBUG("failed or unused jets: ");
   for (JetCIter j = failedJetIters.first; j != failedJetIters.second; ++j) {
     ATH_MSG_DEBUG((*j)->p4().Et() << " " << (*j) -> eta());
   }

   ATH_MSG_DEBUG("conditions: ");
   for (auto c = m_conditions.begin(); c != m_conditions.end(); ++c) {
     ATH_MSG_DEBUG(*c);
   }
  }

 CleanerMatcher TrigHLTJetHypo::getCleanerMatcher() const{
   /* initalise a Cleaner Matcher.
      Cleaning: according to the user set flags.

      Match conditions: SingleEtaRegion and MaximumBipartite matchers are 
      available. For now only hardwire the MaximumBipartite matcher.

   */

   auto cleaner = cleanerFactory(//basic cleaning
				 m_n90Threshold, 
				 m_presamplerThreshold,
				 m_negativeEThreshold,
				 //loose cleaning
				 m_fSampMaxLooseThreshold,
				 m_etaLooseThreshold,
				 m_emfLowLooseThreshold,
				 m_emfHighLooseThreshold,
				 m_hecfLooseThreshold,
				 //tight cleaning
				 m_fSampMaxTightThreshold,
				 m_etaTightThreshold,
				 m_emfLowTightThreshold,
				 m_emfHighTightThreshold,
				 m_hecfTightThreshold,
				 //long-lived particle cleaning
				 m_fSampMaxLlpThreshold,
				 m_negELlpThreshold,
				 m_hecfLlpThreshold,
				 m_hecqLlpThreshold,
				 m_avLarQFLlpThreshold,
				//cleaning mode
                                 m_cleaningAlg);
   auto matcher = matcherFactory(m_conditions, m_matchingAlg);

   return CleanerMatcher(cleaner, matcher);
 }


HLT::ErrorCode
TrigHLTJetHypo::markAndStorePassingJets(const CleanerMatcher & cm,
                                        const xAOD::JetContainer* outJets,
                                        const HLT::TriggerElement* outputTE
                                        ){
  TrigPassBits* bits = HLT::makeTrigPassBits(outJets);
  auto passedJetIters = cm.passedJetIters();

  for(JetCIter i = passedJetIters.first; i != passedJetIters.second; ++i){
    HLT::markPassing( bits, *i, outJets ); 
  }
  
  // attach the trigger bits to the output trigger element
  return  attachBits(outputTE, bits);
}

void
TrigHLTJetHypo::resetCounters(){
  m_cutCounter = -1;
  m_njet = 0.0;
  m_et = -99000.;
  m_eta = -99.;
  m_phi = -99.;
}
