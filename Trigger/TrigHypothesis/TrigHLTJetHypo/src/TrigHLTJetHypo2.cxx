/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTJetHypo2.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo2
//
// AUTHOR:
//
// ********************************************************************
//

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <memory>
#include <limits>
//
// #include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "TrigHLTJetHypo/TrigHLTJetHypo2.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

#include "xAODTrigger/TrigPassBits.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoHelper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/getEtThresholds.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/conditionsFactory2.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsSorter.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/lineSplitter.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SingleJetGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/AllJetsGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IndexedJetsGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGrouper.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/groupsMatcherFactory.h"

#include <map>
#include <fstream> // debugging

float GeV = 1000.;

enum class HypoStrategy{EtaEt, HT, TLA, DijetMassDEta, singlemass};

HypoStrategy getStrategy(const std::string& s){

  std::map<std::string, HypoStrategy> m{
    {"EtaEt", HypoStrategy::EtaEt},
    {"HT", HypoStrategy::HT},
    {"TLA", HypoStrategy::TLA},
    {"DijetMassDEta", HypoStrategy::DijetMassDEta},
    {"singlemass", HypoStrategy::singlemass}
  };

 return m.at(s);  // throws if not found
}


TrigHLTJetHypo2::TrigHLTJetHypo2(const std::string& name,
			       ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {
  
  m_accepted=0;
  m_rejected=0;
  m_errors=0;
  m_timersvc = nullptr;

  declareProperty("doMonitoring", m_doMonitoring = false );
  declareProperty("AcceptAll",      m_acceptAll=false);
  declareProperty("chain_name",      m_chainName="Unknown");

  declareProperty("hypoStrategy", m_hypoStrategy="Unknown");
  // declareProperty("EtThresholds",   m_EtThresholdsInput ); // Default: 40 GeV
  declareProperty("EtThresholds",   m_EtThresholds ); // Default: 40 GeV
  declareProperty("eta_mins",   m_etaMins);
  declareProperty("eta_maxs",   m_etaMaxs);
  declareProperty("asymmetricEtas",   m_asymmetricEtas);

  //TLA style jet indices
  declareProperty("ystar_mins", m_ystarMins);
  declareProperty("ystar_maxs", m_ystarMaxs);
  declareProperty("mass_mins", m_massMins);
  declareProperty("mass_maxs", m_massMaxs);
  declareProperty("jetvec_indices",   m_jetvec_indices);

  //Dimass dEta
  declareProperty("dEta_mins", m_dEtaMins);
  declareProperty("dEta_maxs", m_dEtaMaxs);

  // HT parameter
  declareProperty("htMin", m_htMin = 0);

  // singlemass parameter
  declareProperty("smc_mins", m_JetMassMin);
  declareProperty("smc_maxs", m_JetMassMax);

  // Dimass, Deta parameters
  //declareProperty("invm", m_invm);
  //declareProperty("deta", m_deta);

  // CombinationsGrouper combinations size;
  declareProperty("combinationsSize", m_combinationsSize=0);
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

TrigHLTJetHypo2::~TrigHLTJetHypo2(){
}

HLT::ErrorCode TrigHLTJetHypo2::hltInitialize()
{
  ATH_MSG_INFO("in initialize()");

  HypoStrategy strategy;
  try {
    strategy = getStrategy(m_hypoStrategy);
  } catch(std::out_of_range e){
    ATH_MSG_ERROR(name() << "Unknown hypo strategy: " << m_hypoStrategy);
                   
    return HLT::ErrorCode(HLT::Action::ABORT_JOB,HLT::Reason::BAD_JOB_SETUP);
  }

  // Initialize timing service
  //------------------------------
  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
    ATH_MSG_WARNING(name() << ": Unable to locate TrigTimer Service");
  }

  if (m_timersvc) {
    TrigTimer* tmp = m_timersvc->addItem("TrigHLTJetHypo2.TrigHLTJetHypo2Tot");
    m_timers.push_back(tmp);
  }


  // make and store the jet cleaner
  setCleaner();

  if (!checkStrategy(strategy)){
    return HLT::ErrorCode(HLT::Action::ABORT_JOB,HLT::Reason::BAD_JOB_SETUP);
  }

  if (!setConditions(strategy)){
    return HLT::ErrorCode(HLT::Action::ABORT_JOB,HLT::Reason::BAD_JOB_SETUP);
  }

  if (!setJetGrouper(strategy)){
    return HLT::ErrorCode(HLT::Action::ABORT_JOB,HLT::Reason::BAD_JOB_SETUP);
  }

  if (m_conditions.empty()){
    ATH_MSG_ERROR("Hypo set up with no conditions");
    return HLT::ErrorCode(HLT::Action::ABORT_JOB,HLT::Reason::BAD_JOB_SETUP);
  } 

  auto matcher = groupsMatcherFactory(m_conditions);
  auto helper = TrigHLTJetHypoHelper(m_cleaners, m_grouper, std::move(matcher));


  // print out the TrigHLTJetHypoHelper configuration
  ATH_MSG_INFO("TrigHLTJetHypo2 : TrigHLTJetHypoHelper for chain " 
               << m_chainName);
  std::string line = helper.toString();
  std::vector<std::string> lines = lineSplitter(line, '\n');

  for(auto l : lines){
    ATH_MSG_INFO(l);
  }

  return HLT::OK;
  
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigHLTJetHypo2::hltFinalize(){
  // ----------------------------------------------------------------------

  ATH_MSG_INFO("in finalize()");
  ATH_MSG_INFO("Events accepted/rejected/errors:  "
               << m_accepted  << " / " << m_rejected << " / "<< m_errors);


  /*
  double sd = 0;
  if (m_nCalls > 0 ){
    sd = std::sqrt( (m_chainTimeSquareAv - m_chainTimeAv)/m_chainTimeAv);
  }
  */

  ATH_MSG_INFO("Chain "
               << m_chainName
               <<" TrigHLTHelper duration (us) "
               << m_chainTimeAv
               << " "
               << m_nCalls);
  
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
HLT::ErrorCode TrigHLTJetHypo2::hltExecute(const HLT::TriggerElement* outputTE,
                                         bool& pass) {

  ATH_MSG_DEBUG("Executing " << name() << "...");
  ATH_MSG_DEBUG("outputTE->getId(): " << outputTE->getId());

  pass=false;

  resetCounters();
  const xAOD::JetContainer* outJets(0);
  auto status = getFeature(outputTE, outJets);
  if (status != HLT::OK){ATH_MSG_ERROR("Jet retrieval failed"); return status;}

  status = checkJets(outJets) != HLT::OK;
  if (status != HLT::OK){
    ATH_MSG_ERROR("Jet checks failed"); 
    return status;
  }

  /* copy the jets to a non-constant container */
  std::vector<const xAOD::Jet*> inJets(outJets->begin(), outJets->end());
  
  HypoJetVector hypoJets(outJets->size());

  std::transform(outJets -> begin(),
		 outJets -> end(),
		 hypoJets.begin(),
		 xAODJetAsIJetFactory());
 
  // make clean matcher evey event - this sees the jets of the event.
  auto matcher = groupsMatcherFactory(m_conditions);
  auto helper = TrigHLTJetHypoHelper(m_cleaners, m_grouper, std::move(matcher));

   /* apply cleaning and hypotheis alg */
  ATH_MSG_DEBUG("hypo helper start... " << name() << "...");
  ATH_MSG_DEBUG("no of jets ... " << inJets.size() << "...");

  steady_clock::time_point t =  steady_clock::now();

  try{
    pass = !inJets.empty() && (m_acceptAll || helper.pass(hypoJets));
  } catch(std::exception& e){
    ATH_MSG_ERROR("Exception raised by the TrigHLTJetHypoHelper(: " 
                  << e.what());
    return HLT::ERROR;
  }

  accumulateTime(steady_clock::now() - t);

  ATH_MSG_DEBUG("hypo testing done... " << name() << "...");

  
  publishResult(helper, pass, outJets);
  status = markAndStorePassingJets(helper, outJets, outputTE);
  if (status != HLT::OK){
    ATH_MSG_ERROR("Trig bits storage failed"); 
     return status;}
  
  // delete the xAOD::Jet wrappers
  for(auto i : hypoJets){delete i;}

  return HLT::OK;
}

HLT::ErrorCode TrigHLTJetHypo2::checkJets(const xAOD::JetContainer* outJets){

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
 TrigHLTJetHypo2::publishResult(const TrigHLTJetHypoHelper& helper,
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

   if(m_dumpJets){ 
     writeDebug(pass, helper.passedJets(), helper.failedJets());
   }

   bumpCounters(pass, helper.passedJets().size());
 }


void TrigHLTJetHypo2::bumpCounters(bool pass, int multiplicity){
   if (pass){
     ++m_accepted;
     m_cutCounter=1;
     m_njet = multiplicity;
   } else {
     ++m_rejected;
   }
 }

void TrigHLTJetHypo2::monitorLeadingJet(const xAOD::Jet* jet){
   ATH_MSG_DEBUG("monitoringLeadingJet " << name() << "...");
   m_et = jet->p4().Et();
   m_eta = jet->eta();
   m_phi = jet->phi();
   ATH_MSG_DEBUG("monitoringLeadingJet done " << name() << "...");
 }

void 
 TrigHLTJetHypo2::writeDebug(bool pass,
                             const HypoJetVector& passedJets,
                             const HypoJetVector& failedJets
                             ) const{
  ATH_MSG_INFO("Writing debug start" << name() << "...");
  
  if(pass){
    std::cout<<name()<< " event passed \n";
  } else {
    std::cout<<name()<< " event failed \n";
  }

  for (auto j :  passedJets) {
    auto p4 = j->p4();
    std::cout<<"\nHYPODUMP passed TrigHLTJetHypo2 Et: " 
             << p4.Et() 
             << " eta " 
             << j->eta()
             << " mass "
             << j->m()
             << '\n';
        //     << " px "
        //     << p4.Px()
        //     << " py "
        //     << p4.Py()
        //     << " pz "
        //     << p4.Pz()
        //     << " E "
        //     << p4.E()
        //     << '\n';
  }
  
  for (auto j :  failedJets) {
    auto p4 = j->p4();
    std::cout<<"\nHYPODUMP failed TrigHLTJetHypo2 Et: " 
             << p4.Et() 
             << " eta " 
             << j->eta()
             << " mass "
             << j->m()
             << '\n';

   //          << " px "
   //          << p4.Px()
   //          << " py "
   //          << p4.Py()
   //          << " pz "
   //          << p4.Pz()
   //          << " E "
   //          << p4.E()
   //            << '\n';
  }
  
}

HLT::ErrorCode
TrigHLTJetHypo2::markAndStorePassingJets(const TrigHLTJetHypoHelper& helper,
                                        const xAOD::JetContainer* outJets,
                                        const HLT::TriggerElement* outputTE
                                        ){
  // TrigPassBits* bits = HLT::makeTrigPassBits(outJets);
  std::unique_ptr<xAOD::TrigPassBits> xBits = 
    xAOD::makeTrigPassBits<xAOD::JetContainer>(outJets);

  for(auto i : helper.passedJets())
    {
      // HLT::markPassing( bits, (*outJets)[i->position()], outJets );
      xBits->markPassing((*outJets)[i->position()], outJets, true); 
    }
  
  // attach the trigger bits to the output trigger element
  // return  attachBits(outputTE, bits);
  auto sc = attachFeature(outputTE, xBits.release(), "passbits");
  if(sc != HLT::OK) {
    ATH_MSG_ERROR("Could not store TrigPassBits! ");
    return sc;
  }
  return HLT::OK;
}

void
TrigHLTJetHypo2::resetCounters(){
  m_cutCounter = -1;
  m_njet = 0.0;
  m_et = -99000.;
  m_eta = -99.;
  m_phi = -99.;
}


bool TrigHLTJetHypo2::checkStrategy(HypoStrategy s){

  if (s == HypoStrategy::EtaEt){ return checkEtaEtStrategy();}
  if (s == HypoStrategy::TLA) {return checkTLAStrategy();}
  if (s == HypoStrategy::DijetMassDEta) { return checkDijetMassDEtaStrategy();}
  if (s == HypoStrategy::HT){return checkHTStrategy();}
  if (s == HypoStrategy::singlemass){return checksinglemassStrategy();}

  ATH_MSG_ERROR(name() << ": no strategy checking for " << m_hypoStrategy);
  return false;
}


bool TrigHLTJetHypo2::checkEtaEtStrategy(){
  if (m_EtThresholds.size() != m_etaMins.size() or
      m_EtThresholds.size() != m_etaMaxs.size() or
      m_asymmetricEtas.size() != m_etaMaxs.size()){
      
    ATH_MSG_ERROR(name()
                  << ": mismatch between number of thresholds "
                  << "and eta min, max boundaries or asymmetric eta flags: "
                  << m_EtThresholds.size() << " "
                  << m_etaMins.size() << " "
                  << m_etaMaxs.size() << " "
                  << m_asymmetricEtas.size() << " "
                  );
    
    return false;
  }
  return true;
}

bool TrigHLTJetHypo2::checksinglemassStrategy(){
  if (m_EtThresholds.size() != m_etaMins.size() or
      m_EtThresholds.size() != m_etaMaxs.size() or
      m_JetMassMin.size() != m_EtThresholds.size() or
      m_JetMassMax.size() != m_EtThresholds.size()) {
      
    ATH_MSG_ERROR(name()
                  << ": mismatch between number of thresholds, "
                  << " eta_min, eta_max, JetMassMin, JetMassMax boundaries: "
                  << m_EtThresholds.size() << " "
                  << m_etaMins.size() << " "
                  << m_etaMaxs.size() << " "
                  << m_JetMassMin.size() << " "
                  << m_JetMassMin.size() << " "
                  );
    
    return false;
  }
  return true;
}

bool TrigHLTJetHypo2::checkTLAStrategy(){
    
  if (1 != m_etaMins.size() or
      1 != m_etaMaxs.size() or
      1 != m_ystarMins.size() or
      1 != m_ystarMaxs.size() or
      1 != m_massMins.size() or
      1 != m_massMaxs.size()){
      
      
    ATH_MSG_ERROR(name()
                  << ": size != 1: "
                  << "eta_mins" <<  m_etaMins.size() << " "
                  << "eta_maxs" <<  m_etaMaxs.size() << " "
                  << "ystar_mins" <<  m_ystarMins.size() << " "
                  << "ystar_maxs" <<  m_ystarMaxs.size() << " "
                  << "mass_mins" <<  m_massMins.size() << " "
                  << "mass_maxs" <<  m_massMaxs.size());
    return false;
  }
  return true;
}

bool TrigHLTJetHypo2::checkDijetMassDEtaStrategy(){

  if (not checkEtaEtStrategy()) {return false;}

  bool multOK = m_EtThresholds.size() > 1;

  bool ystarOK = (m_dEtaMins.size() < 2);
  
  bool massOK = (m_massMins.size() < 2);
  
  bool atLeastOne = m_dEtaMins.size() > 0 or m_massMins.size() > 0;
  
  if (not multOK){
    ATH_MSG_ERROR(name() << ": size error, expect >= 2 ");
    ATH_MSG_ERROR(name() << "Et thresholds " << m_EtThresholds);
  }

  if (not ystarOK){
    ATH_MSG_ERROR(name() << ": size error, expect 0 or 1");
    ATH_MSG_ERROR(name() << " dEta_mins " <<  m_dEtaMins.size());
    ATH_MSG_ERROR(name() << " dEta_maxs " <<  m_dEtaMaxs.size());
  }
  
  if (not massOK){
    ATH_MSG_ERROR(name() << ": size error, expect 0 or 1");
    ATH_MSG_ERROR(name() << " mass_mins " <<  m_massMins.size());
    ATH_MSG_ERROR(name() << " mass_maxs " <<  m_massMaxs.size());
  }
  
  
  if(not atLeastOne){
    ATH_MSG_ERROR(name() << " neither mass nor deta limits given");
  }
  
  return multOK and ystarOK and massOK and atLeastOne;
}

bool TrigHLTJetHypo2::checkHTStrategy(){
  if (1 != m_etaMins.size() or
      1 != m_etaMaxs.size() or
      1 != m_EtThresholds.size() or
      0. > m_htMin){
    
    
    ATH_MSG_ERROR(name()
                  << ": size != 1 or htMin illegal: "
                  << "eta_mins" <<  m_etaMins.size() << " "
                  << "eta_maxs" <<  m_etaMaxs.size() << " "
                  << "et thresholds " << m_EtThresholds.size() << " "
                  << "m_htMin " << m_htMin);
    return false;
  }
  return true;
}


void  TrigHLTJetHypo2::setCleaner() {

  if (m_cleaningAlg == "noCleaning"){return;}

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
    
  m_cleaners.push_back(cleanerFactory.make());
}
  

bool TrigHLTJetHypo2::setConditions(HypoStrategy s){
  
  if (s == HypoStrategy::EtaEt){return setEtaEtConditions();}
  if (s == HypoStrategy::TLA){return setTLAConditions();}
  if (s == HypoStrategy::DijetMassDEta){return setDijetMassDEtaConditions();}
  if (s == HypoStrategy::HT) {return setHTConditions();}
  if (s == HypoStrategy::singlemass) {return setsinglemassConditions();}

  ATH_MSG_ERROR(name()
                << ": no specification to set up conditions for hypo strategy: "
                << m_hypoStrategy);

  return false;
}
  

  
bool TrigHLTJetHypo2::setEtaEtConditions(){
  m_conditions = conditionsFactoryEtaEt(m_etaMins,
                                        m_etaMaxs,
                                        m_EtThresholds,
                                        m_asymmetricEtas);
  std::sort(m_conditions.begin(), m_conditions.end(), ConditionsSorter());
  return true;
}


bool TrigHLTJetHypo2::setsinglemassConditions(){

  std::vector<double> m_JetMassMin_d = getStringBoundaries(m_JetMassMin);
  std::vector<double> m_JetMassMax_d = getStringBoundaries(m_JetMassMax);

  m_conditions = conditionsFactorysinglemass(m_etaMins,
                                        m_etaMaxs,
                                        m_EtThresholds,
                                        m_JetMassMin_d,
                                        m_JetMassMax_d);
  std::sort(m_conditions.begin(), m_conditions.end(), ConditionsSorter());
  return true;
}

bool TrigHLTJetHypo2::setTLAConditions(){

  m_conditions = conditionsFactoryTLA(m_etaMins, m_etaMaxs,
                                      m_ystarMins, m_ystarMaxs,
                                      m_massMins, m_massMaxs);

  std::sort(m_conditions.begin(), m_conditions.end(), ConditionsSorter());
  return true;
}

//std::vector<double> getEtThresholds(const std::vector<double>& dEtas,
//                                    const std::vector<double>& etThresholds){
//  // Emulate the python code setup by TM
//  for (auto dEta : dEtas){
//    if (dEta > 0.) {
//      auto etThreshold = 
//        std::min(40000., 
//                 *std::min_element(etThresholds.cbegin(), 
//                                   etThresholds.cend()));
//
//      // The TM python code chooses the Alg class corresponding
//      // to the etThreshold. This class sets the threshold.
//      // It is not clear what happened if the class did not exist - 
//      // probably a crash. Here we let any threshold be  used (ie
//      // no crash on an unknown threshold)
//      return std::vector<double> {etThreshold, etThreshold};
//    }
//  }
//
//  // For invariant mass hypos without dEta cuts
//  return std::vector<double> {0., 0.};
//}

std::vector<double> TrigHLTJetHypo2::getStringBoundaries (const std::vector<std::string>& stv) {

    std::vector<double> JetMassLimit;

    for (auto st : stv){

      //  if (st.find("inf") != std::string::npos) {JetMassLimit.push_back(std::numeric_limits<double>::max());} 
      //  else {JetMassLimit.push_back(std::stod(st)*GeV);}
    
        if (st.find("-INF") != std::string::npos) {JetMassLimit.push_back(std::numeric_limits<double>::lowest());}

        else if (st.find("INF") != std::string::npos) {JetMassLimit.push_back(std::numeric_limits<double>::max());}

        else {JetMassLimit.push_back(std::stod(st)*GeV);}
    
    }

    return JetMassLimit;

}


bool TrigHLTJetHypo2::setDijetMassDEtaConditions(){
  // emulate old behaviour of TriggerMenu to set the min Et for the jets.
  // These limits are being set in the C++ code to discourage
  // firther changes. A new hypo strategy is under development to replce this
  // one.
  auto dmax = std::numeric_limits<double>::max();

  std::vector<double> etaMins {0., 0.};  // default from the run 1 hypo
  std::vector<double> etaMaxs {dmax, dmax}; // C++ default from Run 1
  std::vector<double> etThresholds = getEtThresholds(m_dEtaMins, 
                                                     m_EtThresholds);
  ATH_MSG_DEBUG("in setDijetMassDEtaConditions dEtamins:");
  for(auto em : m_dEtaMins){ATH_MSG_DEBUG(em);}
  ATH_MSG_DEBUG("in setDijetMassDEtaConditions m_EtThresholds:");
  for(auto et : m_EtThresholds){ATH_MSG_DEBUG(et);}
  ATH_MSG_DEBUG("in setDijetMassDEtaConditions etThresholds:");
  for(auto et : etThresholds){ATH_MSG_DEBUG(et);}


  m_conditions = conditionsFactoryDijetEtaMass(etaMins, etaMaxs,
                                               etThresholds,
                                               m_dEtaMins, m_dEtaMaxs,
                                               m_massMins, m_massMaxs);

  std::sort(m_conditions.begin(), m_conditions.end(), ConditionsSorter());
  return true;
}


bool TrigHLTJetHypo2::setHTConditions(){
  // treat the kinematic cut applied to each jet as a cleaner.
  // Wether these arre handled as a cleaner, or in the condition is
  // aribtrary, but handling them with a cleaner makes debugging easier.
  m_cleaners.push_back(makeEtaEtCleaner(m_etaMins[0], 
                                        m_etaMaxs[0], 
                                        m_EtThresholds[0],
                                        std::numeric_limits<double>::max()));

  m_conditions = conditionsFactoryHT(m_htMin);

  std::sort(m_conditions.begin(), m_conditions.end(), ConditionsSorter());
  return true;
}

bool TrigHLTJetHypo2::setJetGrouper(HypoStrategy s){
  
  if (s == HypoStrategy::EtaEt){
    m_grouper = std::make_shared<SingleJetGrouper>();
    return true;
  }

  if (s == HypoStrategy::singlemass){
      m_grouper = std::make_shared<SingleJetGrouper>();
      return true;
  }

  if (s == HypoStrategy::HT){
    m_grouper = std::make_shared<AllJetsGrouper>();
    return true;
  }

  if (s == HypoStrategy::TLA){
    m_grouper = std::make_shared<IndexedJetsGrouper>(m_jetvec_indices);
    return true;
  }


  if (s == HypoStrategy::DijetMassDEta){
    m_grouper = std::make_shared<CombinationsGrouper>(2);
    return true;
  }

  ATH_MSG_ERROR(name()
                << ": no specification to set up conditions for hypo strategy: "
                << m_hypoStrategy);
                
  return false;
}


void  TrigHLTJetHypo2::accumulateTime(nanoseconds duration) noexcept{

  auto dtime = duration_cast<microseconds>(duration);
  auto counts = dtime.count();
  //  countssq = counts*counts;

  if (m_nCalls == 0){
    m_nCalls = 1;
    m_chainTimeAv = counts;
    // m_chainTimeSquareAv = countssq;
    return;
  }
  
  m_nCalls += 1;
  m_chainTimeAv = (m_chainTimeAv * (m_nCalls - 1) +  counts)/m_nCalls;
  //m_chainTimeSquareAv += 
  //  (m_chainTimeSquareAv * (m_nCalls - 1) + countssq)/m_nCalls;
}

  
  

