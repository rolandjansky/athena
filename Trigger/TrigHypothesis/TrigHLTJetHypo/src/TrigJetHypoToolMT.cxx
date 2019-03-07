/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigJetHypoToolMT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "TrigJetHypoToolMT.h"

#include "GaudiKernel/StatusCode.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/conditionsFactory2.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsSorter.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SingleJetGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJetFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/groupsMatcherFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoHelper2.h"

#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"

using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;

TrigJetHypoToolMT::TrigJetHypoToolMT(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  base_class(type, name, parent),
  m_decisionId(HLT::Identifier::fromToolName(name)) {
}


TrigJetHypoToolMT::~TrigJetHypoToolMT(){
}

StatusCode TrigJetHypoToolMT::initialize(){
  // get the conditions objects for the chain.
  // as these hav no state, they can be created once per job.
  m_conditions = m_config->getConditions();
  return StatusCode::SUCCESS;
}

StatusCode TrigJetHypoToolMT::finalize(){
  return StatusCode::SUCCESS;
}

StatusCode TrigJetHypoToolMT::decide(const xAOD::JetContainer* jets,
                                           bool& pass) const {

  pass = false;
  HypoJetVector hypoJets(jets->size());
  
  std::transform(jets -> begin(),
                 jets -> end(),
                 hypoJets.begin(),
                 xAODJetAsIJetFactory());
 
  // make a new TrigHLTJetHypoHelper2 every event
  auto matcher = groupsMatcherFactory(m_conditions);
  auto grouper = m_config->getJetGrouper();
  auto helper = TrigHLTJetHypoHelper2(m_cleaners, 
                                      std::move(grouper), 
                                      std::move(matcher));

  /* apply cleaning and hypothesis alg */
  ATH_MSG_DEBUG("hypo helper start... "
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
  
  ATH_MSG_DEBUG("hypo testing done chain  "
                << " no of input jets " << jets->size()
                << " pass " << pass );
  
  if(m_dumpJets){writeDebug(pass, helper.passedJets(), helper.failedJets());}

  // delete the xAOD::Jet wrappers
  for(auto i : hypoJets){delete i;}

  return StatusCode::SUCCESS;
}



void  TrigJetHypoToolMT::setCleaners() {
  // set up a cleaner factory with known jet cleaners
  // Allow the configurer to add in new jet cleaners.
  
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

  auto cleaners = m_config->getCleaners();  // sub class supplied cleaners
  m_cleaners.insert(m_cleaners.end(), cleaners.begin(), cleaners.end());
  ATH_MSG_INFO("No of Cleaners " << m_cleaners.size());
}

void TrigJetHypoToolMT::writeDebug(bool pass,
                                       const HypoJetVector& passedJets,
                                       const HypoJetVector& failedJets
                                       ) const{
  ATH_MSG_INFO("Writing debug start...");
  
  if(pass){
    ATH_MSG_DEBUG(" event passed");
  } else {
    ATH_MSG_DEBUG(" event failed");
  }

  for (auto j :  passedJets) {
    auto p4 = j->p4();
    ATH_MSG_DEBUG("\nHYPODUMP passed TrigJetHypoToolImp Et: " 
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
		 << '\n');
  }
  
  for (auto j :  failedJets) {
    auto p4 = j->p4();
    ATH_MSG_DEBUG("\nHYPODUMP failed TrigJetHypoToolImp Et: " 
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
		  << '\n');
  }
  
}


const HLT::Identifier& TrigJetHypoToolMT::getId() const{
  return m_decisionId;
} 
