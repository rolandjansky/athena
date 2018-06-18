/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTJet_EtaEtTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR: P Sherwood
//
// ********************************************************************

#include "TrigJetHypoTool_EtaEt.h"

#include "GaudiKernel/StatusCode.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/conditionsFactory2.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsSorter.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SingleJetGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJetFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/groupsMatcherFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"

TrigJetHypoTool_EtaEt::TrigJetHypoTool_EtaEt(const std::string& name):
  asg::AsgTool(name), m_id(name) {
  
  declareProperty("EtThresholds",   m_EtThresholds );
  declareProperty("eta_mins",   m_etaMins);
  declareProperty("eta_maxs",   m_etaMaxs);
  declareProperty("asymmetricEtas",   m_asymmetricEtas);


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
  declareProperty("AverageLArQFLlpThreshold",
                  m_avLarQFLlpThreshold = 0.8*65535);

  
  // Monitored variables...
  /*
  declareMonitoredVariable("NJet", m_njet);
  declareMonitoredVariable("Et", m_et);
  declareMonitoredVariable("Eta", m_eta);
  declareMonitoredVariable("Phi", m_phi);
  */
}


TrigJetHypoTool_EtaEt::~TrigJetHypoTool_EtaEt(){
}

StatusCode TrigJetHypoTool_EtaEt::initialize(){
  return StatusCode::SUCCESS;
}

StatusCode TrigJetHypoTool_EtaEt::finalize(){
  return StatusCode::SUCCESS;
}

StatusCode TrigJetHypoTool_EtaEt::decide(const xAOD::JetContainer* jets,
                                         bool& pass) const{

  HypoJetVector hypoJets(jets->size());

  std::transform(jets -> begin(),
                 jets -> end(),
                 hypoJets.begin(),
                 xAODJetAsIJetFactory());
 
  // make a new CleanerMatcher every event
  auto matcher = groupsMatcherFactory(m_conditions);
  auto grouper = getJetGrouper();
  auto helper = TrigHLTJetHypoHelper2(m_cleaners, 
                                      std::move(grouper), 
                                      std::move(matcher));

  /* apply cleaning and hypothesis alg */
  ATH_MSG_DEBUG("hypo helper start... " << m_chainName
                << " no of jets ... " 
                << jets->size() 
                << "...");

  // steady_clock::time_point t =  steady_clock::now();

  try{
    pass = !jets->empty() && (m_acceptAll || helper.pass(hypoJets));
  } catch(std::exception& e){
    ATH_MSG_ERROR("Exception raised by the TrigHLTJetHypoHelper2: " 
                  << e.what());
    return StatusCode::FAILURE;
  }

  // accumulateTime(steady_clock::now() - t);
  
  ATH_MSG_DEBUG("hypo testing done chain  " << m_chainName
                << " no of input jets " << jets->size()
                << " pass " << pass );
  
  if(m_dumpJets){writeDebug(pass, helper.passedJets(), helper.failedJets());}

  // delete the xAOD::Jet wrappers
  for(auto i : hypoJets){delete i;}

  return StatusCode::SUCCESS;
}



Conditions TrigJetHypoTool_EtaEt::getConditions() const {
  auto conditions = conditionsFactoryEtaEt(m_etaMins,
                                           m_etaMaxs,
                                           m_EtThresholds,
                                           m_asymmetricEtas);
  std::sort(conditions.begin(), conditions.end(), ConditionsSorter());
  
  return conditions;
}

 
std::unique_ptr<IJetGrouper> TrigJetHypoTool_EtaEt::getJetGrouper() const {
  return std::make_unique<SingleJetGrouper>();
}

StatusCode TrigJetHypoTool_EtaEt::checkVals() const {
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
    
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

std::vector<std::shared_ptr<ICleaner>> 
TrigJetHypoTool_EtaEt::getCleaners() const {
  std::vector<std::shared_ptr<ICleaner>> v;
  return v;
}


void  TrigJetHypoTool_EtaEt::setCleaners() {
  
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

void TrigJetHypoTool_EtaEt::writeDebug(bool pass,
                                       const HypoJetVector& passedJets,
                                       const HypoJetVector& failedJets
                                       ) const{
  ATH_MSG_INFO("Writing debug start" << m_chainName << "...");
  
  if(pass){
    std::cout<<m_chainName<< " event passed \n";
  } else {
    std::cout<<m_chainName<< " event failed \n";
  }

  for (auto j :  passedJets) {
    auto p4 = j->p4();
    std::cout<<"\nHYPODUMP passed TrigJetHypoToolImp Et: " 
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
    std::cout<<"\nHYPODUMP failed TrigJetHypoToolImp Et: " 
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


TrigCompositeUtils::DecisionID TrigJetHypoTool_EtaEt::decisionId() const{
  return m_id.numeric();
}  
