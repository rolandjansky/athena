/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration 
*/

#include "TrigBtagEmulationTool/TrigBtagEmulationChain.h"

#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "PathResolver/PathResolver.h"

#include "TSystem.h"
#include "fstream"

// FrameWork includes

#ifdef XAOD_STANDALONE
#include "TrigConfxAOD/xAODConfigTool.h"
#include <AsgTools/MessageCheck.h>

#elif !defined( XAOD_ANALYSIS )
#include "GaudiKernel/Property.h"
#endif


using namespace Trig;

//**********************************************************************

TrigBtagEmulationChain::TrigBtagEmulationChain(const std::vector<std::string>& chainDefinition, ToolHandle<Trig::TrigDecisionTool>& trigDec) 
  : m_name( chainDefinition.size() != 0 ? chainDefinition.at(0) : "NoTriggerName" ),
    m_trigDec(trigDec),
    m_correctlyConfigured(true),
    m_autoConfigured(false) {

  // Parser if only trigger name is specified, w/o sub-triggers
  std::vector< std::string > m_chains( chainDefinition.begin(),chainDefinition.end() );

  // Automatic Parser
  if (m_chains.size() == 1) {
    m_autoConfigured = true;

    std::vector< BaseTrigBtagEmulationChainJetIngredient* > triggerSubComponents;
    if ( !parseChainName( chainDefinition.at(0), triggerSubComponents ) ) 
      m_correctlyConfigured = false;

    for (unsigned int i=0; i<triggerSubComponents.size(); i++)
      addDecisionIngredient( triggerSubComponents.at(i) );

    return;
  }

  // Prepare loop on ingredients
  std::vector<std::string>::const_iterator name=m_chains.begin();
  
  // Loop on ingredents
  for( ; name!=m_chains.end(); name++) {
    if (name == m_chains.begin()) continue;

    // Handle already available L1 or HLT decision
    if( name->find("EMUL_")==std::string::npos )
      addDecisionIngredient(*name);
    // Handle emulated L1
    else if ( name->find("-JJ")!=std::string::npos )
      addDecisionIngredient( new TrigBtagEmulationChainJetIngredient_L1_JJ(*name) );
    else if ( name->find("EMUL_L1")!=std::string::npos ) 
      addDecisionIngredient( new TrigBtagEmulationChainJetIngredient_L1(*name) );
    // Handle emulated HLT
    else if ( name->find("gsc")!=std::string::npos )
      addDecisionIngredient( new TrigBtagEmulationChainJetIngredient_GSC(*name) );
    else if ( name->find("EMUL_HLT")!=std::string::npos )
      addDecisionIngredient( new TrigBtagEmulationChainJetIngredient_HLT(*name) );
    // Handle not recognized cases
    else 
      m_correctlyConfigured = false;
  }

  for (unsigned int i = 0; i < m_ingredientsJet.size(); i++)
    m_ingredientsJet.at(i)->initialize();
}

void TrigBtagEmulationChain::addDecisionIngredient(std::string decision) { m_ingredientsDecision.push_back(decision); }
void TrigBtagEmulationChain::addDecisionIngredient(BaseTrigBtagEmulationChainJetIngredient* decision) { m_ingredientsJet.push_back( decision ); }

bool TrigBtagEmulationChain::parseChainName( std::string triggerName, std::vector< BaseTrigBtagEmulationChainJetIngredient* >& trigger_subComponents) {
  std::vector< std::string > parsedTriggers;

  if ( triggerName.find("_AND_")!=std::string::npos ) {
    std::string trigger_AND_trigA = triggerName.substr( 0,triggerName.find("_AND_") );
    std::string trigger_AND_trigB = "HLT_" + triggerName.substr( triggerName.find("_AND_") + 5, triggerName.length() - triggerName.find("_AND_") - 5 );
    parsedTriggers.push_back( trigger_AND_trigA );
    parsedTriggers.push_back( trigger_AND_trigB );
  } else parsedTriggers.push_back( triggerName );

  for (unsigned int i(0); i<parsedTriggers.size(); i++) {
    triggerName = parsedTriggers.at(i);
    std::vector< BaseTrigBtagEmulationChainJetIngredient* > tmp_trigger_subComponents;

    if (triggerName.find("HLT_")!=std::string::npos) tmp_trigger_subComponents = processHLTtrigger( triggerName );
    else if (triggerName.find("L1_")!=std::string::npos) tmp_trigger_subComponents = processL1trigger( triggerName );
    else m_correctlyConfigured = false;

    trigger_subComponents.insert( trigger_subComponents.end(), tmp_trigger_subComponents.begin(), tmp_trigger_subComponents.end());
  }

  return true;
}

std::vector< BaseTrigBtagEmulationChainJetIngredient* > TrigBtagEmulationChain::processL1trigger (std::string input) {
  std::vector< BaseTrigBtagEmulationChainJetIngredient* > output;

  while ( !input.empty() ) {
    std::string subString = "";

    bool hasSeparation = input.find("_") != std::string::npos;
    subString = hasSeparation ? input.substr( 0,input.find("_") ) : input;
    input = hasSeparation ? input.substr( input.find("_") + 1 , input.length() - input.find("_") -1 ) : "";

    if (subString == "L1") continue;
    else if (subString.find("MJJ")==std::string::npos &&
	     subString.find("JJ")!=std::string::npos) output.push_back( new TrigBtagEmulationChainJetIngredient_L1_JJ("EMUL_L1_" + subString) ); 
    else output.push_back( new TrigBtagEmulationChainJetIngredient_L1("EMUL_L1_" + subString) );
    output.at( output.size() - 1)->initialize();
  }

  return output;
}
std::vector< BaseTrigBtagEmulationChainJetIngredient* > TrigBtagEmulationChain::processHLTtrigger (std::string input) {
  std::vector< BaseTrigBtagEmulationChainJetIngredient* > output;

  std::string L1component  = "";
  std::string HLTcomponent = "";
  if (input.find("L1") != std::string::npos) {
    L1component  = input.substr( input.find("L1") , input.length() - input.find("L1") );
    HLTcomponent = input.substr( 0 , input.find("L1") - 1);
    L1component.replace( L1component.find("L1") + 2, 0 ,"_" );
  }

  // Deal with L1 Components
  if ( !L1component.empty() ) {
    std::vector< BaseTrigBtagEmulationChainJetIngredient* > trigger_L1 = processL1trigger( L1component );
    output.insert( output.end(), trigger_L1.begin(), trigger_L1.end() );
  }

  // Deal with HLT components
  std::vector< std::string > outputHLT_string;
  if ( !L1component.empty() ) input = HLTcomponent;

  while ( !input.empty() ) {
    std::string subString = "";

    bool hasSeparation = input.find("_")!=std::string::npos;
    subString = hasSeparation ? input.substr( 0,input.find("_") ) : input;
    input = hasSeparation ? input.substr( input.find("_") + 1 , input.length() - input.find("_") -1 ) : "";

    if (subString == "HLT") continue;
    else if (outputHLT_string.size() == 0) outputHLT_string.push_back( "EMUL_HLT_" + subString );
    else if (subString.find("j") != std::string::npos) outputHLT_string.push_back( "EMUL_HLT_" + subString );
    else outputHLT_string.at(outputHLT_string.size() - 1) +=  "_"+subString;
  }

  std::vector< TrigBtagEmulationChainJetIngredient_HLT* > triggers_HLT;
  for (unsigned int i(0); i<outputHLT_string.size(); i++) {
    if (outputHLT_string.at(i).find("gsc")!=std::string::npos) triggers_HLT.push_back( new TrigBtagEmulationChainJetIngredient_GSC( outputHLT_string.at(i) ) );
    else triggers_HLT.push_back( new TrigBtagEmulationChainJetIngredient_HLT( outputHLT_string.at(i) ) );
  }
  for (unsigned int i(0); i<triggers_HLT.size(); i++)
    triggers_HLT.at(i)->initialize();
  
  // Deal with overlapping HLT triggers
  for (unsigned int i(0); i<triggers_HLT.size(); i++)
    for (unsigned int j(i+1); j<triggers_HLT.size(); j++) {
      if (triggers_HLT.at(i)->contains( triggers_HLT.at(j) ) ) triggers_HLT.at(i)->addJetsMulteplicity(triggers_HLT.at(j));
      else if (triggers_HLT.at(j)->contains( triggers_HLT.at(i) ) ) triggers_HLT.at(j)->addJetsMulteplicity(triggers_HLT.at(i));
      else if (triggers_HLT.at(i)->overlaps( triggers_HLT.at(j) ) ) m_correctlyConfigured = false;
    }
 
  output.insert( output.end(), triggers_HLT.begin(), triggers_HLT.end() );
  return output;
}

std::string TrigBtagEmulationChain::getName() const { return m_name; }
bool TrigBtagEmulationChain::isCorrectlyConfigured() const { return m_correctlyConfigured; } 
bool TrigBtagEmulationChain::isAutoConfigured() const { return m_autoConfigured; }

std::vector< std::string > TrigBtagEmulationChain::retrieveAutoConfiguration() const {
  std::vector< std::string > output( m_ingredientsDecision.begin(),m_ingredientsDecision.end() );
  for (unsigned int i=0; i<m_ingredientsJet.size(); i++)
    output.push_back( m_ingredientsJet.at(i)->getName() );
  return output;
}

void TrigBtagEmulationChain::evaluate() {
  for (unsigned int index(0); index<m_ingredientsJet.size(); index++)
    m_ingredientsJet.at(index)->evaluate();
}

bool TrigBtagEmulationChain::isPassed() {
  std::vector<std::string>::iterator dec, decEnd=m_ingredientsDecision.end();
  for(dec=m_ingredientsDecision.begin(); dec!=decEnd; dec++)
    if(!m_trigDec->isPassed(*dec)) return false;

  for (unsigned int index(0); index<m_ingredientsJet.size(); index++)
    if ( !m_ingredientsJet.at(index)->isPassed() ) return false;

  return true;
}

void TrigBtagEmulationChain::Print() {
  std::cout<<std::endl<<"### TRIGGER EMULATION ###"<<std::endl;
  for (unsigned int i=0; i<m_ingredientsJet.size();i++)
    m_ingredientsJet.at(i)->Print();
}

bool TrigBtagEmulationChain::hasFeature(std::string feature) {
  for (unsigned int index(0); index<m_ingredientsJet.size(); index++)
    if ( m_ingredientsJet.at(index)->hasFeature(feature) ) return true;
  return false;
}

void TrigBtagEmulationChain::clear() {
  for (unsigned int index(0); index<m_ingredientsJet.size(); index++)
    m_ingredientsJet.at(index)->clear();
}

bool TrigBtagEmulationChain::addJet(std::string item,std::vector< TrigBtagEmulationJet* >& jets) {
  for (unsigned int index(0); index<m_ingredientsJet.size(); index++)
    if (m_ingredientsJet.at(index)->needsJet(item)) m_ingredientsJet.at(index)->addJet(item,jets);
  return true;
}

//**********************************************************************
