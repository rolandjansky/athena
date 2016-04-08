/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTJetHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:
//
// ********************************************************************
//

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <memory>
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
// #include "GaudiKernel/ListItem.h"

// #include "TrigSteeringEvent/Enums.h"
#include "TrigHLTJetHypo/TrigHLTJetHypo.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

// #include "JetUtils/JetCaloQualityUtils.h"
// #include "CLHEP/Units/SystemOfUnits.h"

#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/matcherFactoryFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/conditionsFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsSorter.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/lineSplitter.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

// #include <memory>

TrigHLTJetHypo::TrigHLTJetHypo(const std::string& name,
			       ISvcLocator* pSvcLocator):
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

  //TLA style jet indices
  declareProperty("ystar_mins", m_ystarMins);
  declareProperty("ystar_maxs", m_ystarMaxs);
  declareProperty("mass_mins", m_massMins);
  declareProperty("mass_maxs", m_massMaxs);
  declareProperty("jetvec_indices",   m_jetvec_indices);

  // cleaning
  declareProperty("cleaningAlg", m_cleaningAlg = "noCleaning");

  // matching. Legal: maximumBipartite, orderedCollections, selectedJets
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
  declareProperty("negativeELlpThreshold", m_negELlpThreshold = 10e3 ); // 10 GeV
  declareProperty("HECfLlpThreshold", m_hecfLlpThreshold = 0.5 );
  declareProperty("HECQLlpThreshold", m_hecqLlpThreshold = 0.5 );
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

  std::vector<std::string> v {"orderedCollections", "maximumBipartite"};
  if (std::find(v.begin(), v.end(), m_matchingAlg) != v.end()){

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
  }

  if (m_matchingAlg == "selectedJets") {
    if (m_etaMins.size() != m_etaMaxs.size() or
	m_etaMins.size() != m_ystarMins.size() or
	m_etaMins.size() != m_ystarMaxs.size() or
	m_etaMins.size() != m_massMins.size() or
	m_etaMins.size() != m_massMaxs.size()){


      msg() << MSG::ERROR
	    << name()
	    << ": size mismatch: "
	    << "eta_mins" <<  m_etaMins.size() << " "
	    << "eta_maxs"  <<  m_etaMaxs.size() << " "
	    << "ystar_mins" <<  m_ystarMins.size() << " "
	    << "ystar_maxs" <<  m_ystarMaxs.size() << " "
	    << "mass_mins" <<  m_massMins.size() << " "
	    << "mass_maxs" <<  m_massMaxs.size() << " "
	    << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB,HLT::Reason::BAD_JOB_SETUP);
    }
  }

  if(m_matchingAlg == "orderedCollections"){
    if (std::set<double>(m_etaMins.cbegin(), m_etaMins.cend()).size() != 1){
      return HLT::ErrorCode(HLT::Action::ABORT_JOB,HLT::Reason::BAD_JOB_SETUP);
    }      
    if (std::set<double>(m_etaMaxs.cbegin(), m_etaMaxs.cend()).size() != 1){
      return HLT::ErrorCode(HLT::Action::ABORT_JOB,HLT::Reason::BAD_JOB_SETUP);
    }      
  }

  std::vector<std::string> vv {"orderedCollections", "maximumBipartite"};

  Conditions conditions;
  std::shared_ptr<IMatcherFactory> matcherFactory;

  if (std::find(vv.begin(), vv.end(), m_matchingAlg) != vv.end()){
    conditions =  conditionsFactory(m_etaMins,
				    m_etaMaxs,
				    m_EtThresholds);
    std::sort(conditions.begin(), conditions.end(), ConditionsSorter());
    matcherFactory = matcherFactoryFactory(conditions, m_matchingAlg);
  } else if (m_matchingAlg == "selectedJets") {
    conditions =  conditionsFactory(m_etaMins, m_etaMaxs,
				    m_ystarMins, m_ystarMaxs,
				    m_massMins, m_massMaxs);
    std::sort(conditions.begin(), conditions.end(), ConditionsSorter());
    matcherFactory = matcherFactoryFactory(conditions,
					   m_jetvec_indices,
					   m_matchingAlg);
  } else {
    msg() << MSG::ERROR
          << name()
          << ": unknown key to set up the matcher factory: "
	  << m_matchingAlg
          << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB,HLT::Reason::BAD_JOB_SETUP);
  }

   CleanerFactory cleanerFactory(//basic cleaning
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
				 m_cleaningAlg);
   
   // m_cleaner = cleanerFactory.make();

   m_cleanerMatcherFactory = 
     std::make_shared<CleanerMatcherFactory>(cleanerFactory,
					     matcherFactory);

  // Later code assumes conditions vector is not empty.
  if (conditions.empty()){
    ATH_MSG_ERROR("TrigHLTJetHypo bad configuration, no conditions given");
    return HLT::ERROR;
  }
    

  // print out the CleanerMatcher configuration
  ATH_MSG_INFO("TrigHLTJetHypo cleanerMatcher: ");
  std::string line = (m_cleanerMatcherFactory->make()).toString();
  std::vector<std::string> lines = lineSplitter(line, '\n');

  for(auto l : lines){
    ATH_MSG_INFO(l);
  }

  return HLT::OK;
  
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigHLTJetHypo::hltFinalize(){
  // ----------------------------------------------------------------------

  msg() << MSG::INFO << "in finalize()" << endreq;
  msg() << MSG::INFO << "Events accepted/rejected/errors:  "<< m_accepted <<" / "<<m_rejected<< " / "<< m_errors<< endreq;
  return HLT::OK;
}

class xAODJetAsIJetFactory{
  /* Create a HypoJet::IJet from a xAODJet while remembering index info
     (for example position in a Data vector of the xAODJet) */

public:
  xAODJetAsIJetFactory(): m_ind(-1){}
  pHypoJet operator() (const xAOD::Jet* j){
    return new HypoJet::xAODJetAsIJet(j, ++m_ind);}
private:
  unsigned int m_ind;
};
  
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
  
  HypoJetVector hypoJets(outJets->size());

  std::transform(outJets -> begin(),
		 outJets -> end(),
		 hypoJets.begin(),
		 xAODJetAsIJetFactory());
 
  // auto cleanerMatcher = getCleanerMatcher();
  auto cleanerMatcher = m_cleanerMatcherFactory->make();

   /* apply cleaning and hypotheis alg */
  ATH_MSG_DEBUG("cleaning and Matching start... " << name() << "...");
  ATH_MSG_DEBUG("no of jets ... " << theJets.size() << "...");
  try{
    // cleanerMatcher.process(theJets.begin(), theJets.end());
    cleanerMatcher.process(hypoJets);
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
  
  // delete the xAOD::Jet wrappers
  for(auto i : hypoJets){delete i;}

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
 TrigHLTJetHypo::publishResult(const CleanerMatcher & cm,
			       bool pass,
			       const xAOD::JetContainer*& outJets){
   ATH_MSG_DEBUG("Publishing " << name() << "...");

   /* The jets used to satisfy the hypo conditions may not contain 
      the leading jet, so find this now */

   if(pass){
     auto leading_jet = std::max_element(outJets->begin(),
                                 outJets->end(),
                                 [](const xAOD::Jet* j0,
                                    const xAOD::Jet* j1){
                                   return j0->p4().Et() < j1->p4().Et();});

     monitorLeadingJet(*leading_jet);
   }

   writeDebug(pass, cm.passedJets(), cm.failedJets(), cm);
   bumpCounters(pass, cm.passedJets().size());
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
                            const HypoJetVector& passedJets,
                            const HypoJetVector& failedJets,
			    const CleanerMatcher& cm
                            ) const{
    ATH_MSG_DEBUG("Writing debug start" << name() << "...");

    if(pass){ATH_MSG_DEBUG("Event accepted");}
    else { ATH_MSG_DEBUG("Event rejected");}

    for (auto j :  passedJets) 
      {
	ATH_MSG_DEBUG(j->p4().Et() << " " << j->eta());
      }

   ATH_MSG_DEBUG("failed or unused jets: ");
   for (auto j : failedJets)
     {
       ATH_MSG_DEBUG(j->p4().Et() << " " << j -> eta());
     }
   
   ATH_MSG_DEBUG("conditions: ");
   for (auto c : cm.getConditions()) {
     ATH_MSG_DEBUG(c.toString());
   }
   ATH_MSG_DEBUG("Writing debug end" << name() << "...");

  }


HLT::ErrorCode
TrigHLTJetHypo::markAndStorePassingJets(const CleanerMatcher & cm,
                                        const xAOD::JetContainer* outJets,
                                        const HLT::TriggerElement* outputTE
                                        ){
  TrigPassBits* bits = HLT::makeTrigPassBits(outJets);

  for(auto i : cm.passedJets())
    {
      HLT::markPassing( bits, (*outJets)[i->position()], outJets ); 
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
