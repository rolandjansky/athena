/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTJetHypoBase.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <limits>
//
// #include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoBase.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

#include "xAODTrigger/TrigPassBits.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoHelper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/getEtThresholds.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/lineSplitter.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/groupsMatcherFactory.h"

#include <map>
#include <fstream> // debugging

using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::nanoseconds;


TrigHLTJetHypoBase::TrigHLTJetHypoBase(const std::string& name,
			       ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {

  m_accepted=0;
  m_rejected=0;
  m_errors=0;
  m_timersvc = nullptr;

  declareProperty("doMonitoring", m_doMonitoring = false );
  declareProperty("AcceptAll",      m_acceptAll=false);
  declareProperty("chain_name",      m_chainName="Unknown");

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
 

  // Monitored variables...
  declareMonitoredVariable("CutCounter", m_cutCounter);

  declareMonitoredVariable("NJet", m_njet);
  declareMonitoredVariable("Et", m_et);
  declareMonitoredVariable("Eta", m_eta);
  declareMonitoredVariable("Phi", m_phi);

}

TrigHLTJetHypoBase::~TrigHLTJetHypoBase(){
}

HLT::ErrorCode TrigHLTJetHypoBase::hltInitialize()
{
  ATH_MSG_INFO("in initialize()");

  // Initialize timing service
  //------------------------------
  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
    ATH_MSG_WARNING(name() << ": Unable to locate TrigTimer Service");
  }

  if (m_timersvc) {
    TrigTimer* tmp = m_timersvc->addItem("TrigHLTJetHypoBase.TrigHLTJetHypoBaseTot");
    m_timers.push_back(tmp);
  }

  // make and store the jet cleaners
  setCleaners();

  
  if (!checkVals()){
    return HLT::ErrorCode(HLT::Action::ABORT_JOB,HLT::Reason::BAD_JOB_SETUP);
  }

  setConditions();
  setJetGrouper();

  if (m_conditions.empty()){
    ATH_MSG_ERROR("Hypo set up with no conditions");
    return HLT::ErrorCode(HLT::Action::ABORT_JOB,HLT::Reason::BAD_JOB_SETUP);
  } 

  auto matcher = groupsMatcherFactory(m_conditions);
  auto helper = TrigHLTJetHypoHelper(m_cleaners, m_grouper, std::move(matcher));


  // print out the TrigHLTJetHypoHelper configuration
  ATH_MSG_INFO("TrigHLTJetHypoBase : TrigHLTJetHypoHelper for chain " 
               << m_chainName);
  std::string line = helper.toString();
  std::vector<std::string> lines = lineSplitter(line, '\n');

  for(auto l : lines){
    ATH_MSG_INFO(l);
  }

  return HLT::OK;
  
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigHLTJetHypoBase::hltFinalize(){
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
HLT::ErrorCode TrigHLTJetHypoBase::hltExecute(const HLT::TriggerElement* outputTE,
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

HLT::ErrorCode TrigHLTJetHypoBase::checkJets(const xAOD::JetContainer* outJets){

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
 TrigHLTJetHypoBase::publishResult(const TrigHLTJetHypoHelper& helper,
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


void TrigHLTJetHypoBase::bumpCounters(bool pass, int multiplicity){
   if (pass){
     ++m_accepted;
     m_cutCounter=1;
     m_njet = multiplicity;
   } else {
     ++m_rejected;
   }
 }

void TrigHLTJetHypoBase::monitorLeadingJet(const xAOD::Jet* jet){
   ATH_MSG_DEBUG("monitoringLeadingJet " << name() << "...");
   m_et = jet->p4().Et();
   m_eta = jet->eta();
   m_phi = jet->phi();
   ATH_MSG_DEBUG("monitoringLeadingJet done " << name() << "...");
 }

void 
 TrigHLTJetHypoBase::writeDebug(bool pass,
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
    std::cout<<"\nHYPODUMP passed TrigHLTJetHypoBase Et: " 
             << p4.Et() 
             << " eta " 
             << j->eta() 
             << " px "
             << p4.Px()
             << " py "
             << p4.Py()
             << " pz "
             << p4.Pz()
             << " E "
             << p4.E()
             << '\n';
  }
  
  for (auto j :  failedJets) {
    auto p4 = j->p4();
    std::cout<<"\nHYPODUMP failed TrigHLTJetHypoBase Et: " 
             << p4.Et() 
             << " eta " 
             << j->eta() 
             << " px "
             << p4.Px()
             << " py "
             << p4.Py()
             << " pz "
             << p4.Pz()
             << " E "
             << p4.E()
               << '\n';
  }
  
}

HLT::ErrorCode
TrigHLTJetHypoBase::markAndStorePassingJets(const TrigHLTJetHypoHelper& helper,
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
TrigHLTJetHypoBase::resetCounters(){
  m_cutCounter = -1;
  m_njet = 0.0;
  m_et = -99000.;
  m_eta = -99.;
  m_phi = -99.;
}


void  TrigHLTJetHypoBase::setCleaners() {

  if (m_cleaningAlg != "noCleaning"){

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

  auto cleaners = getCleaners();  // sub class supplied cleaners
  m_cleaners.insert(m_cleaners.end(), cleaners.begin(), cleaners.end());
  ATH_MSG_INFO("No of Cleaners " << m_cleaners.size());
}
  

void  TrigHLTJetHypoBase::accumulateTime(nanoseconds duration) noexcept{

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


void  TrigHLTJetHypoBase::setConditions() {
  m_conditions = getConditions();
}

void  TrigHLTJetHypoBase::setJetGrouper() {
  m_grouper = getJetGrouper();
}

  
  

