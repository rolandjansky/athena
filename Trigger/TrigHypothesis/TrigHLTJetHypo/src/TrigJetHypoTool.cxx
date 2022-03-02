/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigJetHypoTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "TrigJetHypoTool.h"

#include "GaudiKernel/StatusCode.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJetFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/lineSplitter.h"
#include "./DebugInfoCollector.h"
#include "./xAODJetCollector.h"

#include "TrigCompositeUtils/TrigCompositeUtils.h"

#include "AthenaMonitoringKernel/Monitored.h"


#include <sstream>

using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::isLegId;
using TrigCompositeUtils::getIDFromLeg;
using TrigCompositeUtils::createLegName;


TrigJetHypoTool::TrigJetHypoTool(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_decisionID(HLT::Identifier::fromToolName(name))
{
}


TrigJetHypoTool::~TrigJetHypoTool(){
}

StatusCode TrigJetHypoTool::initialize(){

  if (!(m_endLabelIndex > 0)){
    ATH_MSG_ERROR("endLabelIndex must be > 0, is " + std::to_string(m_endLabelIndex));
    return StatusCode::FAILURE;
  }
  
  if (m_endLabelIndex == 1u) {

    // This is a single-leg jet chain.
    m_decisionIDs.push_back(m_decisionID);
  
  } else {

    // This is a multi-leg chain with (m_endLabelIndex - m_startLabelIndex) jet legs contained within

    if(not isLegId(m_decisionID)){
      ATH_MSG_ERROR(name() << " is being configured to do jet selection for a multi-leg chain, from leg " << m_startLabelIndex <<
        " to " << m_endLabelIndex << ", the HypoTool's name must therefore start with legXXX_ rather than HLT_");
      return StatusCode::FAILURE;
    }

    HLT::Identifier noLegNameDecisionID = getIDFromLeg(m_decisionID);
    for (std::size_t i = m_startLabelIndex; i != m_endLabelIndex; ++i){
      m_decisionIDs.push_back(createLegName(noLegNameDecisionID, i));
      ATH_MSG_DEBUG("Adding jet-leg " << m_decisionIDs.size()-1 << " for " << noLegNameDecisionID << ", this is leg " << i << " in the chain:" << m_decisionIDs.back());
    }

  }
  
  if (m_visitDebug){
    DebugInfoCollector collector(name() + "_init");
    CHECK(m_helper->getDescription(collector));
    auto s = collector.toString();
    
    for(const auto& l : lineSplitter(s)){
      ATH_MSG_INFO(l);
    }
  }
  
  if (!m_monTool.empty()) ATH_CHECK(m_monTool.retrieve());
  return StatusCode::SUCCESS;
}


StatusCode TrigJetHypoTool::finalize(){
  return StatusCode::SUCCESS;
}


StatusCode
TrigJetHypoTool::decide(const xAOD::JetContainer* jets,
                        const TrigCompositeUtils::DecisionIDContainer& previousDecisionIDs,
                        std::vector<JetDecision>& jetHypoInputs) const {

  // need only check against the decision ID corresponding to the name
  // of this hypo. Leg indices are irrelevant at this point.
  if (not TrigCompositeUtils::passed(m_decisionID.numeric(),
             previousDecisionIDs)) {
    return StatusCode::SUCCESS;
  }

  std::unique_ptr<ITrigJetHypoInfoCollector> infocollector(nullptr);
  std::unique_ptr<ITrigJetHypoInfoCollector> jetdumper(nullptr);
  if(m_visitDebug){
    auto collectorName = name() + "_" + std::to_string(m_id++);
    infocollector.reset(new  DebugInfoCollector(collectorName));
    auto jetdumperName =
      name()+"_passingjets_" + std::to_string(m_id++);
    jetdumper.reset(new  DebugInfoCollector(jetdumperName));
  }
  
   
  HypoJetVector hypoJets(jets->size());
  
  std::transform(jets -> begin(),
                 jets -> end(),
                 hypoJets.begin(),
                 xAODJetAsIJetFactory());

  /* apply cleaning and hypothesis alg */
  ATH_MSG_DEBUG("hypo helper start... "
                << " no of jets ... " 
                << jets->size() 
                << "...");

  // steady_clock::time_point t =  steady_clock::now();
  // monitoring -- timing plots (filled for every call)
  auto tHypo = Monitored::Timer<std::chrono::milliseconds>("TIME_jetHypo");
  auto mon_NInputs =  Monitored::Scalar("NJetsIn",jets->size()); 
  auto mon_NOutputs =  Monitored::Scalar("NJetsOut",-1);
  auto monitor_group_multTime = Monitored::Group( m_monTool, tHypo, mon_NOutputs, mon_NInputs);

  xAODJetCollector jetCollector;
  bool pass;
  try{
    pass = m_helper->pass(hypoJets, jetCollector, infocollector);
  } catch(std::exception& e){
    ATH_MSG_ERROR("Exception raised by the TrigJetHypoToolHelper: " 
                  << e.what());
    return StatusCode::FAILURE;
  }
  
  if (!pass) {
    
    if (infocollector){
        
      std::string msg =
        "hypo testing done: no of input jets " + std::to_string(jets->size())
        + " no of participating jets " + std::to_string(jetCollector.size())
        +  " pass: false ";
      
      infocollector->collect("TrigJetHypoTool", msg);
    }
    
    return StatusCode::SUCCESS;
  }
  
  CHECK(checkPassingJets(jetCollector, infocollector));
  CHECK(reportPassingJets(jetCollector, jetHypoInputs));
  
  // accumulateTime(steady_clock::now() - t);

  
  if (infocollector){

    std::string msg =
      "hypo testing done: no of input jets " + std::to_string(jets->size())
      + " no of particlating jets " + std::to_string(jetCollector.size())
      +  " pass: true";

    
    infocollector->collect("TrigJetHypoTool", msg);
    infocollector->write();

    std::stringstream ss;
    ss << jetCollector.hypoJets();
    jetdumper->collect("passed", ss.str());
  }
  tHypo.stop();
  
  //monitoring -- filled in passing events 
  HypoJetVector hjv = jetCollector.hypoJets();

  mon_NOutputs = hjv.size();
  for(const auto& j : hjv) {
    auto mon_jetEt =   Monitored::Scalar("Et",     j->et()*0.001);
    auto mon_jetEta =  Monitored::Scalar("Eta",    j->eta());
    auto mon_jetPhi =  Monitored::Scalar("Phi",    j->phi());
    auto mon_jetMass = Monitored::Scalar("Mass",   j->m()*0.001);
    auto monitor_group_passingjets = Monitored::Group( m_monTool, mon_jetEt, mon_jetEta, mon_jetPhi );       
  }
  //monitor the passing jets for each leg (there should only be one per chain!)
  auto legInds = jetCollector.legInds();
  for (const auto& label : legInds) {
    auto jets = jetCollector.hypoJets(label);
    auto monitor_nJt = Monitored::Scalar( "NJets", jets.size());      
    auto htsum =  Monitored::Scalar("HT", std::accumulate(jets.begin(), jets.end(),0.0,
							  [](double sum, const HypoJetVector::value_type jptr){return sum + jptr->et()*0.001;} ));      
    auto monitor_group_passinght = Monitored::Group(m_monTool, monitor_nJt,htsum);
  }
  
  return StatusCode::SUCCESS;
}


const std::vector<HLT::Identifier>& TrigJetHypoTool::getIDs() const{
  return m_decisionIDs;
}


const HLT::Identifier& TrigJetHypoTool::getID(std::size_t i) const{
  return m_decisionIDs.at(i-m_startLabelIndex);
}
    

StatusCode
TrigJetHypoTool::checkPassingJets(const xAODJetCollector& jetCollector,
                                  const std::unique_ptr<ITrigJetHypoInfoCollector>& infocollector) const {
  
  if (jetCollector.empty()) {
    ATH_MSG_ERROR("HypoTool passed the event for " <<
      m_chainName << " ev " << m_id <<
      ", but did not specify which jets participated");
    return StatusCode::FAILURE;
  }

  auto legIndices = jetCollector.legInds();
  if (!(legIndices.size() == (m_endLabelIndex - m_startLabelIndex))) {
    ATH_MSG_ERROR("inconsistent number of leg indices");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("check passing jets? does it work?");
  ATH_MSG_DEBUG("There are some indices: " << legIndices.size());
  for(size_t i = 0; i < legIndices.size(); i ++){
    ATH_MSG_DEBUG(" -- leg " << i << " has index " << legIndices.at(i));
  }
  
  // jet hypo inputs:
  // pairs of const xAOD::Jet* (first) and mutable Decision* (second)
  
  auto participating_jets = jetCollector.xAODJets();
  if (infocollector) {
    infocollector->
      collect(name(),
      "no of xAODJets " + std::to_string(participating_jets.size()));
    
    auto legInds = jetCollector.legInds();
    std::stringstream ss;
    
    for(const auto& label : legInds){
      auto jets = jetCollector.xAODJets(label);
      ss << label << " [\n";
      for(const auto& j : jets){
        ss << static_cast<const void*>(j) << '\n';
      }
      ss << "]\n";
    }
    infocollector->collect(name(), ss.str());
  }
  
  return StatusCode::SUCCESS;
}

StatusCode
TrigJetHypoTool::reportPassingJets(const xAODJetCollector& jetCollector,
                                   const std::vector<JetDecision>& jetHypoInputs) const {

  auto legIndices = jetCollector.legInds();
  for(const auto& legInd : legIndices){
    auto participating_jets = jetCollector.xAODJets(legInd);
    CHECK(reportLeg(participating_jets, jetHypoInputs, legInd));
  }

  return StatusCode::SUCCESS;
} 




StatusCode
TrigJetHypoTool::reportLeg(const std::vector<const xAOD::Jet*>& jets,
                           const std::vector<JetDecision>& jetHypoInputs,
                           int legInd) const {

  // check if input jet is a participating jet. If so , add
  // its Decision object.
  for (auto& pair : jetHypoInputs) {
    if (!inputJetParticipates(jets, pair)){continue;}

    TrigCompositeUtils::DecisionIDContainer passingIDs; // set<uint32_t>

    // Add the Leg DecisionID.
    // Required by any downstream Steps which need to consume jets which
    // are satisfying this leg's jet cuts and by the following ComboHypo
    // Note that if the chain is single-leg, then the ID will be the chain-ID
    // rathe than a leg-specific ID
    passingIDs.insert(getID(legInd).numeric());

    // Copy these passingIDs into the Decision Object
    // This call also performs de-duplication
    TrigCompositeUtils::insertDecisionIDs(passingIDs, pair.second);
  }

  return StatusCode::SUCCESS;
}


bool
TrigJetHypoTool::inputJetParticipates(const std::vector<const xAOD::Jet*>& jets,
                                      const JetDecision& pair) const  {
  
  return std::find(jets.begin(), jets.end(), pair.first) != jets.end();
}
