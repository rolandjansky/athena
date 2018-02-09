/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/DiTauProcessorTool.h"

#include "xAODTau/DiTauJetContainer.h"
#include "xAODTau/DiTauJetAuxContainer.h"


//________________________________________
DiTauProcessorTool::DiTauProcessorTool(const std::string& type) :
  asg::AsgTool(type),
  m_sDiTauContainerNameHadHad("DiTauJets"),
  m_sDiTauContainerNameHadEl("DiTauJetsHadEl"),
  m_sDiTauContainerNameHadMu("DiTauJetsHadMu")
{
  declareProperty("DiTauContainerHadHad", m_sDiTauContainerNameHadHad);
  declareProperty("DiTauContainerHadEl", m_sDiTauContainerNameHadEl);
  declareProperty("DiTauContainerHadMu", m_sDiTauContainerNameHadMu);
  declareProperty("VarCalculatorTools", m_aVarCalculatorTools, "List of IDiTauVarCalculator tools");
}

//________________________________________
DiTauProcessorTool::~DiTauProcessorTool(){}

//________________________________________
StatusCode DiTauProcessorTool::initialize(){
  for(auto varCalculator : m_aVarCalculatorTools){
    if(varCalculator->initialize().isFailure()){
      ATH_MSG_ERROR("Failed initializing DiTauVarCalculatorTool "<<varCalculator->name());
      return StatusCode::FAILURE;
    }
    std::string decayChannel = *(dynamic_cast<asg::AsgTool*>(&*varCalculator)->getProperty<std::string>("DiTauDecayChannel"));
    if(decayChannel == "HadHad"){
      m_aVarCalculatorToolsHadHad.push_back(varCalculator);
      continue;
    }
    if(decayChannel == "HadEl"){
      m_aVarCalculatorToolsHadEl.push_back(varCalculator);
      continue;
    }
    if(decayChannel == "HadMu"){
      m_aVarCalculatorToolsHadMu.push_back(varCalculator);
      continue;
    }
    ATH_MSG_ERROR("VariableCalculatorTool" << varCalculator->name() << "has no valid DecayModePropery.");
    return StatusCode::FAILURE;
  }
  
  //-------------------------------------------------------------------------
  // No tools allocated!
  //-------------------------------------------------------------------------
  if (m_aVarCalculatorTools.empty()) {
    ATH_MSG_ERROR("no tools given!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//________________________________________
StatusCode DiTauProcessorTool::execute(){
  StatusCode sc = StatusCode::SUCCESS;
  if(!m_aVarCalculatorToolsHadHad.empty())
    sc = sc && processContainer(m_sDiTauContainerNameHadHad, m_aVarCalculatorToolsHadHad);
  if(!m_aVarCalculatorToolsHadEl.empty())
    sc = sc && processContainer(m_sDiTauContainerNameHadEl, m_aVarCalculatorToolsHadEl);
  if(!m_aVarCalculatorToolsHadMu.empty())
    sc = sc && processContainer(m_sDiTauContainerNameHadMu, m_aVarCalculatorToolsHadMu);
  return sc;
}

StatusCode DiTauProcessorTool::processContainer(std::string& diTauContainerName, 
                                                ToolHandleArray<tauRecTools::IDiTauToolBase>& varCalculatorTools){
  
  const xAOD::DiTauJetContainer*     pContainer(0);

  StatusCode sc;
  
  //-------------------------------------------------------------------------
  // retrieve Di-Tau Containers from StoreGate
  //-------------------------------------------------------------------------
  sc = evtStore()->retrieve(pContainer, diTauContainerName);
  if (sc.isFailure()) {
      ATH_MSG_WARNING("Failed to retrieve " << diTauContainerName << "! Will exit TauProcessor now!!");
      return StatusCode::FAILURE;
  }

  for(auto xDiTau : *pContainer){
    for(auto varCalculator : varCalculatorTools){
      if(varCalculator->execute(*xDiTau).isFailure()){
        ATH_MSG_ERROR("Failed to execute() for Tool" <<varCalculator->name());
        return StatusCode::FAILURE;
      }
    }
  }

  return StatusCode::SUCCESS;

}

//________________________________________
StatusCode DiTauProcessorTool::finalize(){

  return StatusCode::SUCCESS;
}
