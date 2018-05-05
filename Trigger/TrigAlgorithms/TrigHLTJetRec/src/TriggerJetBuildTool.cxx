/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigHLTJetRec/TriggerJetBuildTool.h"

// string->inputtype
#include "xAODJet/JetContainerInfo.h"

// #include "xAODJet/JetAuxContainer.h"
#include "xAODJet/JetTrigAuxContainer.h"
#include "JetRec/PseudoJetContainer.h"
#include "TrigHLTJetRec/PseudoJetCreatorFunctions.h"
#include "JetRec/IParticleExtractor.h"
// #include "JetRec/constituentsDEBUG.h"  // DEBUG REMOVE ME
#include <string>
#include <sstream>
#include <iostream>


#include "JetEDM/JetConstituentFiller.h"  // DEBUG FIXME

using xAOD::JetContainer;

//**********************************************************************
TriggerJetBuildTool::TriggerJetBuildTool(const std::string& name): 
  AsgTool(name), m_finder("") {
  declareProperty("JetFinder", m_finder);
  declareProperty("JetModifiers", m_modifiers);
  declareProperty("NoNegE", m_noNegE);
  // declareProperty("label", m_label);
  declareProperty("concrete_type", m_concreteTypeStr);
  declareProperty("iParticleRejecter", m_iParticleRejecter);
}

//**********************************************************************

StatusCode TriggerJetBuildTool::initialize() {
  ATH_MSG_INFO("Initializing TriggerJetBuildTool " << name() << ".");

  bool modsOK{true};
  // m_inputType = xAOD::JetInput::inputType(m_label); 
  m_concreteType = xAOD::JetInput::inputType(m_concreteTypeStr); 
  
  // PS Modifiers: need to undrestand if modifiers are needed.
  // if so - how to handle calls to inputContainerNames and
  // setPseudojetRetriever

  // Fetch the IParticleRejecter tool.
  if (m_iParticleRejecter.retrieve().isSuccess()) {
    ATH_MSG_INFO("Retrieved IParticle rejecter "
                 << m_iParticleRejecter->name());
  } else {
    ATH_MSG_ERROR("Unable to retrieve  IParticle rejecter");
  }
    
  ATH_MSG_INFO(name() << " has " << m_modifiers.size() << " jet modifiers.");
  
  // Fetch the jet modifiers.
  ATH_MSG_INFO(name() << " has " << m_modifiers.size() << " jet modifiers.");
    
  for(const auto& hmod : m_modifiers){
    
    if (hmod.retrieve().isSuccess()) {
      ATH_MSG_INFO("Retrieved " << hmod->name());
    } else {
      ATH_MSG_ERROR("Unable to retrieve IJetModifier");
      modsOK = false;
    }

    // note - no calls to inPutContainer or setPseudoJetRetriever
    // use dummy defaults
    // hmod->inputContainerNames(m_incolls);
    // hmod->setPseudojetRetriever(m_ppjr);
  }

  return modsOK ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

//**********************************************************************

StatusCode TriggerJetBuildTool::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

//**********************************************************************

const JetContainer* TriggerJetBuildTool::build() const {
  ATH_MSG_DEBUG("Entering build...");

  // Make a new empty jet collection to write to.
  JetContainer* pjets = new JetContainer;
  pjets->setStore(new xAOD::JetTrigAuxContainer);
  // pjets->setStore(new xAOD::JetAuxContainer);

  ATH_MSG_DEBUG("Created jet container, set aux container...");

  // obtain the input Pseudojets (converted from IParticles)
  auto ppseudoJetContainer = getInputPseudoJetContainer();

  ATH_MSG_DEBUG("Obtained PseudojetContainer with "
                << (ppseudoJetContainer->casVectorPseudoJet())->size()
                << " pseudojets");
  
  m_finder->findNoSave(*ppseudoJetContainer, *pjets, m_concreteType);

  ATH_MSG_DEBUG(pjets->size() 
                << " jets reonstructed from " 
                << ppseudoJetContainer->size()
                <<" inputs");


  for(const xAOD::Jet* j : *pjets){
    ATH_MSG_DEBUG("no of constituents "
                  << (j->getConstituents()).size()
                  << " " << j->numConstituents()
                  << " no of pseudojet constituents "
                  << (jet::JetConstituentFiller::constituentPseudoJets(*j)).size()
                  <<'\n');
  }
  

  ATH_MSG_DEBUG(ppseudoJetContainer->toString(1, 1));
 
  if (pjets->empty()) {return pjets;}

  
  ATH_MSG_DEBUG("Executing " << m_modifiers.size() << " jet modifiers.");
  ATH_MSG_DEBUG("First jet E: " << (pjets->front())->e()
                << " " << (pjets->front())->eta());

  
  for (const auto& mod : m_modifiers){
    ATH_MSG_DEBUG("  Executing modifier " << mod->name());
    try{ //FIXME remove try after debugging
      mod->modify(*pjets);
    } catch(...) {
      ATH_MSG_ERROR("Error executing jet modifier " <<mod->name());
    }

    ATH_MSG_DEBUG("First jet E: " << (pjets->front())->e()
                  << "  " << (pjets->front())->eta()
                  );
  }

  ATH_MSG_DEBUG("Exiting build...");
  return pjets;

}


void TriggerJetBuildTool::prime(const xAOD::IParticleContainer* inputs){

  
  // use the input IParticles to make a PseudoJEtContainer.
  // This method may be called more than once per event
  // m_label is a string which gives type information. It is used
  // to determine the function used to select the incoming IParticles.
  ATH_MSG_DEBUG("Entering prime(), call " << ++m_nprime);

  constexpr bool isGhost = false;
  IParticleExtractor* extractor = new IParticleExtractor(inputs,
                                                         m_concreteTypeStr,
                                                         isGhost,
                                                         m_isTrigger);

  
  ATH_MSG_DEBUG("No of IParticle inputs: " << inputs->size());
  for(const auto& ip : *inputs){
    ATH_MSG_VERBOSE("PseudoJetInputDump " 
                    << ip->e() 
                    << " "
                    <<xAOD::JetInput::typeName(m_concreteType)
                    << " "
                    << m_concreteTypeStr);
  }
  
  /*  DEBUG FIXEME remove the following commented out code
  constexpr bool noRejection = true;
  std::vector<PseudoJet> vpj = 
    PseudoJetCreatorFunctions::createPseudoJets(inputs, 
                                                m_concreteType, 
                                                m_noNegE,
                                                noRejection
                                                );
  */
  
  std::vector<PseudoJet> vpj = 
    PseudoJetCreatorFunctions::createPseudoJets(inputs, 
                                                m_concreteType,
                                                m_iParticleRejecter
                                                );
  
  ATH_MSG_DEBUG("No of pseudojets: " << vpj.size());
  for(const auto& pj : vpj){
    ATH_MSG_VERBOSE("PseudoJetDump " 
                    << pj.Et() 
                    << " "
                    <<xAOD::JetInput::typeName(m_concreteType));
  }

  ATH_MSG_DEBUG("So far this job IParticles"
                << " tested " << m_iParticleRejecter->tested()
                << " rejected " << m_iParticleRejecter->rejected());
                
                

  PseudoJetContainer pjc(extractor, vpj);
  std::cerr<< "TriggerJetBuildTool::prime() appending PseudoJetContainer\n";
  m_inContainer.append(&pjc);
  std::cerr<< "TriggerJetBuildTool::prime() end\n";
}


const PseudoJetContainer*
TriggerJetBuildTool::getInputPseudoJetContainer() const {
  return &m_inContainer;
}


void TriggerJetBuildTool::resetInputs(){
  // allow Algotithm to empty out exising Psudojets before
  // accumlating new ones.
  ATH_MSG_DEBUG("Entering resetInputs()");

  m_inContainer = PseudoJetContainer();
  m_nprime = 0;
}


void TriggerJetBuildTool::print() const {

  asg::AsgTool::print();  
  ATH_MSG_INFO("  Concrete Type " <<  m_concreteTypeStr);
  ATH_MSG_INFO("  Modifiers [" << m_modifiers.size() << "]");
  ATH_MSG_INFO("  JetFinder: " << m_finder->name());
  m_finder->print();
  ATH_MSG_INFO("  Modifier count is " << m_modifiers.size());
  for(auto m : m_modifiers){
    ATH_MSG_INFO("-----------");        
    m->print();    
  }
  
  ATH_MSG_INFO(getInputPseudoJetContainer()->toString(0));
  ATH_MSG_INFO("End print for " << name());
}


std::string TriggerJetBuildTool::toString(int verbosity) const{
  std::ostringstream os;
  os << "TriggerJetBuildTool: " << name() << " " << this << '\n';
  os<< "   concrete type str " << m_concreteTypeStr << '\n';
  os<< "   concrete type " << xAOD::JetInput::typeName(m_concreteType) << '\n';
  os << "  Modifiers [" << m_modifiers.size() << "]\n";
  for(auto m : m_modifiers){os << "    " << m.name() << '\n';}
  os << "  JetFinder: " << m_finder->name() << '\n'; 
  os << getInputPseudoJetContainer()->toString(verbosity);
  return os.str();
}

std::string TriggerJetBuildTool::dumpPseudoJets() const{
  return getInputPseudoJetContainer()->dumpPseudoJets();
}



