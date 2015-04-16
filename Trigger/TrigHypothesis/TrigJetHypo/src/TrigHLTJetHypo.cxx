/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTJetHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigEFCaloHypo
//
// AUTHOR:
//
// ********************************************************************
//

/* WORK IN PROGRESS _ DO NOT USE ! */

#include <algorithm>
#include <sstream>
#include <cmath>
#include <exception>
#include <memory>
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"

#include "TrigSteeringEvent/Enums.h"
#include "TrigJetHypo/TrigHLTJetHypo.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

#include "JetUtils/JetCaloQualityUtils.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "TrigSteeringEvent/TrigPassBits.h"


struct DescendingEt:std::binary_function<const xAOD::Jet*,
                                         const xAOD::Jet*,
                                         bool> {
  bool operator()(const xAOD::Jet* l, const xAOD::Jet* r)  const {
    return l->p4().Et() > r->p4().Et();
  }
};


template<typename Cleaner, typename Matcher>
class CleanerMatcher: public ICleanerMatcher{
public:
  CleanerMatcher(const Cleaner& cleaner, Matcher& matcher):ICleanerMatcher(),
    m_matcher(matcher), m_cleaner(cleaner){}
  void process(JetIter begin, JetIter end) {
    JetIter cleanEnd = std::partition(begin, end, m_cleaner);
    m_matcher.match(begin, cleanEnd);
  }

  bool pass() const{return m_matcher.pass();}

  std::pair<JetCIter, JetCIter> 
  passedJetIters() const {return m_matcher.passed_iters();}

  std::pair<JetCIter, JetCIter> 
  failedJetIters() const {return m_matcher.failed_iters();}
  
private:
  Matcher m_matcher;
  Cleaner m_cleaner;
};


class SingleEtaRegionMatcher {
public:
  ConditionsSorter sorterPred;
  SingleEtaRegionMatcher(const Conditions& cs):m_conditions(cs), m_pass(false){

    if(not std::is_sorted(m_conditions.begin(), m_conditions.end(), sorterPred))
      {
        std::sort(m_conditions.begin(), m_conditions.end(), sorterPred);
      }

    Condition first = m_conditions[0];
    for (auto c : m_conditions) {
      if (c.etaMin() != first.etaMin()){
        throw std::runtime_error("more than 1 eta region");
      }
      if (c.etaMax() != first.etaMax()){
        throw std::runtime_error("more than 1 eta region");
      }
    }
  }
  
  void match(JetIter b, JetIter e){
    // sort jets by descending eta

    m_passed_jets.clear(); // allow monitoring of the jets that pass conditions.
    m_pass = true;

    // select jets in the eta range - which is common for all conditions
    const Condition& c = m_conditions[0];

    e = std::partition(b, e, [&](const xAOD::Jet* j){return c.etaOK(j);});

    int n_conds = m_conditions.size();
    int n_jets = e - b;

    if (n_jets < n_conds){
      std::sort (b, e, DescendingEt());
      m_failed_jets.clear();
    }else{
      /* Only the first n_conds jets from the ordered jet vector
         can contribute to the success of the event. Copy the rest to 
         the bad jet vector */
      std::partial_sort (b, b + n_conds, e, DescendingEt());
      m_failed_jets.assign(b + n_conds,  e);
    }

    // zip the conditions and jet collection iterators
    // with length being the shorter of the two collections.
    using IterPair = std::pair<ConditionsIter, JetCIter>;

    std::vector<IterPair> zip;
    std::size_t zipLen = std::min(n_conds, n_jets);

    for(std::size_t i = 0; i != zipLen; ++i) {
      zip.push_back(IterPair(m_conditions.begin() + i, b + i));}

    /* while there are are more jet-condition pairs, check whether
       the jet matches the condition. If it does, and there has
       been no previous failure, add it to the good jets vector.
       Otherwise add it to the bad jets vector */

    for(auto iters : zip){
      auto ci = iters.first; auto ji = iters.second;
      if(m_pass){
        if (ci->allOK((*ji))){m_passed_jets.push_back(*ji);}
        else {
          m_pass = false;
          m_failed_jets.push_back(*ji);}
      } else {
        m_failed_jets.push_back(*ji);
      }
    }        
  }
  bool pass() const {return m_pass;}
  std::pair<JetCIter, JetCIter> passed_iters() const {
    return  std::pair<JetCIter, JetCIter> (m_passed_jets.begin(),
                                           m_passed_jets.end());}
  std::pair<JetCIter, JetCIter> failed_iters() const {
    return  std::pair<JetCIter, JetCIter> (m_failed_jets.begin(),
                                           m_failed_jets.end());}
private:
  Conditions m_conditions;
  bool m_pass;
  JetVector m_passed_jets;
  JetVector m_failed_jets;
};


struct UncleanableJet: public std::runtime_error{
  UncleanableJet(std::string s):runtime_error(s.c_str()){}    
};


class BasicCleaner{
public:
  BasicCleaner(float n90Threshold,
               float presamplerThreshold,
               float negativeEThreshold):
    m_n90Threshold(n90Threshold), 
    m_presamplerThreshold(presamplerThreshold),
    m_negativeEThreshold(negativeEThreshold){
  }
  
  bool operator()(const xAOD::Jet* jet) const {
    /* make cuts on jet attributes to select clean jets */
    
    bool isClean{true};

    float n90jet;
    if (jet -> getAttribute<float>("n90", n90jet)){
      if(n90jet < m_n90Threshold){isClean = false;}
    } else {
      throw UncleanableJet("Cleaner Cannot retrieve n90");
    }
      
    float presamplerfractionjet;
    if(jet -> getAttribute<float>("presamplerFraction",
                                  presamplerfractionjet)){
      if(presamplerfractionjet > m_presamplerThreshold) {isClean = false;}
    } else {
      throw UncleanableJet("Cleaner Cannot retrieve presamplerFraction");
    }
    	
    float negEjet;
    if(jet -> getAttribute<float>("NegativeE", negEjet)){
        if(negEjet < m_negativeEThreshold){isClean = false;}
    } else {
      throw UncleanableJet("Cleaner Cannot retrieve NegativeE");
    }
    
    return isClean;

  }

private:
  float m_n90Threshold;
  float m_presamplerThreshold;
  float m_negativeEThreshold;
};

// class AntiBasicCleaner: public BasicCleaner {
class AntiBasicCleaner: public BasicCleaner {
public:
  AntiBasicCleaner(float n90Threshold,
                   float presamplerThreshold,
                   float negativeEThreshold):
    BasicCleaner(n90Threshold, presamplerThreshold, negativeEThreshold){}

  bool operator()(const xAOD::Jet* jet) const{
    return !BasicCleaner::operator()(jet);
  }
};

class NullCleaner{
public:
  bool operator()(const xAOD::Jet*) const {return true;}    
};


TrigHLTJetHypo::TrigHLTJetHypo(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("Etcut",   m_EtCut = 40*CLHEP::GeV); // Default: 40 GeV
  declareProperty("etaMincut",   m_etaMinCut = 0 ); // Default: 0
  declareProperty("etaMaxcut",   m_etaMaxCut = 10 ); // Default: 10
  declareProperty("Multiplicitycut", m_multiplicitycut=1);
  declareProperty("doMonitoring", m_doMonitoring = false );
  declareProperty("AcceptAll",      m_acceptAll=false);
  //declareProperty("histoPath", m_path = "/stat/Monitoring/EventFilter" );

  declareProperty("EtThresholds",   m_EtThresholdsInput ); // Default: 40 GeV
  declareProperty("eta_mins",   m_etaMins);
  declareProperty("eta_maxs",   m_etaMaxs);
 
  // cleaning
  declareProperty("doBasicCleaning", m_doBasicCleaning = false );
  declareProperty("triggerOnBad", m_triggerOnBad = false );
  
  declareProperty("n90CleaningThreshold", m_n90Threshold = 2 );
  declareProperty("presamplerCleaningThreshold", m_presamplerThreshold = 0.9 );
  declareProperty("negativeECleaningThreshold", m_negativeEThreshold = -60e3 );	// 60 GeV
  declareProperty("qmeanCleaningThreshold", m_qmeanThreshold = 0.8 );
  declareProperty("HECQCleaningThreshold", m_hecQThreshold = 0.5 );
  declareProperty("HECfCleaningThreshold", m_hecFThreshold = 0.5 );
  declareProperty("LArQCleaningThreshold", m_larQThreshold = 0.8 );
  declareProperty("EMfCleaningThreshold", m_emFThreshold = 0.95 );

  declareMonitoredVariable("CutCounter", m_cutCounter);

  // Monitored variables...
  declareMonitoredVariable("NJet", m_njet);
  declareMonitoredVariable("Et", m_et);
  declareMonitoredVariable("Eta", m_eta);
  declareMonitoredVariable("Phi", m_phi);
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigHLTJetHypo::~TrigHLTJetHypo()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NECESSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////
//

// ----------------------------------------------------------------------
HLT::ErrorCode TrigHLTJetHypo::hltInitialize()
  // ----------------------------------------------------------------------
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

  if ( m_EtThresholdsInput.empty() ) { 
    for ( unsigned i=0 ; i<m_multiplicitycut ; ++i ){
      m_EtThresholds.push_back(m_EtCut);
    }      
  } else { 
    for  ( unsigned i=0 ; i<m_EtThresholdsInput.size() ; ++i ){
      m_EtThresholds.push_back(m_EtThresholdsInput[i]);
    }

    if ( m_EtThresholds.size()!=m_multiplicitycut ) {
      /// if not enough thresholds for the required number of jets... 
      /// pad with zeros to the required number of thresholds just in case... 
      for  ( unsigned i=m_EtThresholds.size() ; i<m_multiplicitycut ; ++i ){
        m_EtThresholds.push_back(0);
      }
      
      msg() << MSG::ERROR
            << name()
            << ": mismatch between number of jets required and number of thresholds: "
            << m_multiplicitycut 
            << " jets requested but only "
            << m_EtThresholdsInput.size()
            << " provided."
            << endreq;    
  
      return HLT::ErrorCode(HLT::Action::ABORT_JOB,HLT::Reason::BAD_JOB_SETUP);
    }
    
    bool sorted = true;
    for  ( unsigned i=1 ; i<m_EtThresholds.size() ; ++i ){
      if ( m_EtThresholds[i]>m_EtThresholds[i-1] ) { 
        sorted = false;
        msg() << MSG::WARNING
              << name()
              << ": Threshold "
              << i 
              << " (" 
              << m_EtThresholds[i] 
              << " MeV) greater that threshold " 
              << i-1 
              << " (" 
              << m_EtThresholds[i-1]
              << " MeV), thresholds should be in DECREASING order "
              << endreq;    
      }
    }

    if  ( !sorted ) { 
      msg() << MSG::WARNING 
            << name() 
            << ": sorting thresholds into reverse order" 
            << endreq; 
      std::sort( m_EtThresholds.begin(), m_EtThresholds.end() );
      std::reverse( m_EtThresholds.begin(), m_EtThresholds.end() );
    }
  }
  
  for ( unsigned i=0 ; i<m_EtThresholds.size() ; ++i ) {
    msg() << MSG::INFO 
          << name() 
          << "\t EtThreshold[" 
          << i 
          << "]=" 
          << m_EtThresholds[i] 
          << " MeV" 
          << endreq;
  }
  
  m_accepted=0;
  m_rejected=0;
  m_errors=0;
  
  /*
  for (std::size_t i = 0; i != m_EtThresholds.size(); ++i){
    m_conditions.push_back(Condition(m_etaMins[i],
                                     m_etaMaxs[i],
                                     m_EtThresholds[i]));
  }
  */
  for (std::size_t i = 0; i != m_EtThresholds.size(); ++i){
    m_conditions.push_back(Condition(m_etaMinCut,
                                     m_etaMaxCut,
                                     m_EtThresholds[i]));
  }
  std::sort(m_conditions.begin(), m_conditions.end(), ConditionsSorter());
  

  ATH_MSG_WARNING("Conditions: "<< m_conditions.size());
  for(auto c: m_conditions){ATH_MSG_WARNING(c);}


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
  auto cleanerMatcher = getCleanerMatcher();
  
  /* apply cleaning and hypotheis alg */
  cleanerMatcher -> process(theJets.begin(), theJets.end());

  publishResult(cleanerMatcher);
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
    return HLT::OK;
  }
}


void 
TrigHLTJetHypo::publishResult(const std::unique_ptr<ICleanerMatcher> & cm){
  auto pass = cm -> pass();
  auto passedJetIters = cm -> passedJetIters();
  auto failedJetIters = cm -> failedJetIters();
  
  if(pass){monitorLeadingJet(*passedJetIters.first);}

  pass = (pass || m_acceptAll);
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
  m_et = jet->p4().Et();
  m_eta = jet->eta();
  m_phi = jet->phi();
}
 
 void 
TrigHLTJetHypo::writeDebug(bool pass,
                           int multiplicity,
                           const std::pair<JetCIter, JetCIter>& passedJetIters,
                           const std::pair<JetCIter, JetCIter>& failedJetIters
                           ) const{
  // Et Cut
   if(pass){ATH_MSG_DEBUG("Event accepted");}
   else { ATH_MSG_DEBUG("Event rejected");}


  ATH_MSG_DEBUG("found multiplicity: "
                << multiplicity
                << "\t multiplicity cut: "
                << m_multiplicitycut
                << ("passed Jet Ets:"));

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

std::unique_ptr<ICleanerMatcher> TrigHLTJetHypo::getCleanerMatcher() const{
  /* initalise a Cleaner Matcher.
     Cleaning: according to the user set flags.
     Match conditions: for now only ensure jet Et is above St threshold
     for jets and thresholds ordered in Et for  single region is available.
     
     More complex algorithms (eg efficient multi-et regions with no 
     jet sharing) would be selected here when available.
  */

  // anti-select result of basic cleaning, single eta region
  using ASRcm = CleanerMatcher<AntiBasicCleaner, SingleEtaRegionMatcher>;
  // basic cleaning, single eta region
  using BSRcm = CleanerMatcher<BasicCleaner, SingleEtaRegionMatcher>;
  // no cleaning basic, single eta region
  using NSRcm = CleanerMatcher<NullCleaner, SingleEtaRegionMatcher>;
  
  SingleEtaRegionMatcher matcher(m_conditions);

  
  if (m_doBasicCleaning){
    if (m_triggerOnBad){
      
      AntiBasicCleaner cleaner (m_n90Threshold, 
                                m_presamplerThreshold,
                                m_negativeEThreshold);
  
      return std::unique_ptr<ICleanerMatcher> (new ASRcm(cleaner, matcher));

    } else {
      BasicCleaner cleaner (m_n90Threshold, 
                            m_presamplerThreshold,
                            m_negativeEThreshold);

      return std::unique_ptr<ICleanerMatcher> (new BSRcm(cleaner, matcher));
    }
  }

  // No cleaning
  NullCleaner cleaner;
  return std::unique_ptr<ICleanerMatcher> (new NSRcm(cleaner, matcher));
}


HLT::ErrorCode
TrigHLTJetHypo::markAndStorePassingJets(const std::unique_ptr<ICleanerMatcher> & cm,
                                        const xAOD::JetContainer* outJets,
                                        const HLT::TriggerElement* outputTE
                                        ){
  TrigPassBits* bits = HLT::makeTrigPassBits(outJets);
  auto passedJetIters = cm -> passedJetIters();

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
