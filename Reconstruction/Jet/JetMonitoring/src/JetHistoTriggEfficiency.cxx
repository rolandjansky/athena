/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "JetMonitoring/JetHistoTriggEfficiency.h"

#include "JetMonitoring/JetMonitoringAlg.h"
#include "TrigDecisionInterface/Conditions.h"


JetHistoTriggEfficiency::JetHistoTriggEfficiency( const std::string& type,  const std::string & name ,const IInterface* parent):
  AthAlgTool( type, name, parent )
  , m_selectTool(this)
  , m_jetVar(this)
{
  declareInterface<IJetHistoFiller>(this);

  declareProperty("Selector",m_selectTool);
  declareProperty("Var",m_jetVar);
    
}


StatusCode JetHistoTriggEfficiency::initialize() {
  ATH_MSG_INFO("  initialize "    );

  if(m_selectTool.isEnabled()){
    ATH_CHECK(m_selectTool.retrieve());
    ATH_MSG_INFO( " pre-select jets with "<< m_selectTool->name() );
  } else {m_selectTool.disable();}

  ATH_CHECK(m_jetVar.retrieve());
  
  return StatusCode::SUCCESS;
}

StatusCode JetHistoTriggEfficiency::processJetContainer(const JetMonitoringAlg& parentAlg, const xAOD::JetContainer & jets, const EventContext& ctx) const {
  if(jets.empty()) return StatusCode::SUCCESS;

  // we assume this tool is invoked in an alg selecting a reference trigger.
  ATH_MSG_DEBUG(" working on "<< m_probeTrigChain);
  // check our probe trigger is enabled 
  static SG::AuxElement::ConstAccessor< std::vector< std::string > > acc_disabledTriggers("disabledTriggers");
  auto eventInfo = parentAlg.GetEventInfo(ctx);
  if( acc_disabledTriggers.isAvailable( *eventInfo ) ) {
    const std::vector<std::string> & disabledTriggers = acc_disabledTriggers( *eventInfo );
    for( const std::string & s: disabledTriggers ) if (s==m_probeTrigChain) return StatusCode::SUCCESS;
  }

  // check trigger bits
  const unsigned int probeBits = parentAlg.isPassedBits(m_probeTrigChain);
  if(!(probeBits & TrigDefs::L1_isPassedAfterVeto) ) return StatusCode::SUCCESS;
  if( (probeBits & TrigDefs::EF_prescaled) ) return StatusCode::SUCCESS;


  // Find the probe jet.
  //  --> choose the nth jet passing the pre-selection (where n=m_jetIndex)
  const xAOD::Jet* probeJet = nullptr;
  size_t npassed=0;
  bool doJetSelect = m_selectTool.isEnabled();
  if (!m_sortJets) {
    for(const xAOD::Jet* j: jets){
      if( doJetSelect && !m_selectTool->keep(*j) ) continue;
      if(npassed==m_jetIndex) {probeJet = j; break; } // found ! 
      npassed++;
    }
  } else { //first sort jets according to variable passed, then select the nth jet from the sorted container
    std::list<const xAOD::Jet*> tmpList;
    for(const xAOD::Jet* j: jets){
      if( doJetSelect && !m_selectTool->keep(*j) ) continue;
      tmpList.push_back(j);
    }
    auto sort = [this] (const xAOD::Jet * j1, const xAOD::Jet * j2) {return m_jetVar->value(*j1) > m_jetVar->value(*j2); } ;
    tmpList.sort( sort );
    for(const xAOD::Jet* j : tmpList ) {
      if(npassed==m_jetIndex) {probeJet = j; break; } // found ! 
      npassed++;
    }
  }

  if(probeJet ==nullptr) return StatusCode::SUCCESS;

  // trigger passed ?

  bool trigPassed = parentAlg.isPassed(m_probeTrigChain);
  ATH_MSG_DEBUG( " trigPassed "<< trigPassed << "  " << m_jetVar->value(*probeJet) );
  // ask the framework to fill our TEfficiency
  auto trigpassS= Monitored::Scalar<bool>("trigPassed",trigPassed);

  auto jetVarS= Monitored::Scalar<float>("jetVar",
                                        m_jetVar->value(*probeJet) );
  
  parentAlg.fill(m_group,trigpassS, jetVarS ); 
  
  return StatusCode::SUCCESS;
}


