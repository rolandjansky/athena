/*
Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration 
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

bool TrigBtagEmulationChain::hasBeenCleared   = true;
bool TrigBtagEmulationChain::hasBeenEvaluated = false;
std::map< std::string, std::unique_ptr< BaseTrigBtagEmulationChainJetIngredient > > TrigBtagEmulationChain::m_chains = std::map< std::string, std::unique_ptr< BaseTrigBtagEmulationChainJetIngredient > >();

//**********************************************************************

TrigBtagEmulationChain::TrigBtagEmulationChain( MsgStream &msg,const std::string &name,ToolHandle<Trig::TrigDecisionTool> &trigDecTool )
  : m_name( name ),
    m_correctlyConfigured( true ),
    m_autoConfigured( true ),
    m_trigDec( trigDecTool ),
    m_msg( msg ) 
{
  parseChainName();
  recordChain();
}

TrigBtagEmulationChain::TrigBtagEmulationChain( MsgStream &msg,const std::string &name,const std::vector< std::vector< std::string > > &definition,ToolHandle< Trig::TrigDecisionTool > &trigDecTool )
  : m_name( name ),
    m_definition( definition.begin(),definition.end() ),
    m_correctlyConfigured( true ),
    m_autoConfigured( false ),
    m_trigDec( trigDecTool ),
    m_msg( msg ) 
{
  recordChain();
}

TrigBtagEmulationChain::TrigBtagEmulationChain( const TrigBtagEmulationChain &other ) 
  : m_name( other.m_name ),
    m_definition( other.m_definition.begin(),other.m_definition.end() ),
    m_correctlyConfigured( other.m_correctlyConfigured ),
    m_autoConfigured( other.m_autoConfigured ),
    m_trigDec( other.m_trigDec ),
    m_msg( other.m_msg ),
    m_ingredientsDecision( other.m_ingredientsDecision.begin(),other.m_ingredientsDecision.end() ) {}

TrigBtagEmulationChain::~TrigBtagEmulationChain() {}

//********************************************************************** 

std::string TrigBtagEmulationChain::name() const { return m_name; }
std::vector< std::vector< std::string > > TrigBtagEmulationChain::definition() const { return m_definition; }

bool TrigBtagEmulationChain::parseChainName() {
  std::string input = m_name;

  std::vector< std::vector< std::string > > definition;
  definition.push_back( std::vector< std::string >() );

  while ( not input.empty() ) {
    std::string token = "";
    if ( input.find("_AND_") == std::string::npos &&
	 input.find("__OR__") == std::string::npos ) token = "";
    if ( input.find("_AND_") != std::string::npos && input.find("__OR__") != std::string::npos )
      token = input.find("_AND_") < input.find("__OR__")  ? "_AND_" : "__OR__";
    else if ( input.find("_AND_") != std::string::npos ) token = "_AND_";
    else if ( input.find("__OR__") != std::string::npos ) token = "__OR__";
    
    std::string subToken = token.empty() ? input : input.substr( 0,input.find( token.c_str() ) );
    if ( not token.empty() ) 
      input = input.substr( input.find( token.c_str() ) + token.length(), input.length() - input.find( token.c_str() ) - token.length() );
    else input = "";

    // Correct subToken adding L1 or HLT when not present at the beginning
    if ( subToken.find("HLT") == std::string::npos &&
	 subToken.find("L1")  == std::string::npos ) {
      subToken.insert( 0, subToken.find("J") != std::string::npos ? "L1_" : "HLT_");
    } else if ( subToken.find("L1") != std::string::npos  &&
		subToken.find("L1") != 0 ) {
      subToken.insert( 0,"HLT_" );
    } 

    definition.at( definition.size() - 1 ).push_back( subToken ) ;
    
    if ( token == "__OR__" ) definition.push_back( std::vector< std::string >() );
  }

  for ( unsigned int i(0); i<definition.size(); i++ ) {
    m_definition.push_back( std::vector< std::string >() );
    for ( unsigned int j(0); j<definition.at(i).size(); j++ )
      if ( not parseChainName( definition.at(i).at(j) ) ) return false;
  }

  return true;
}

MsgStream& TrigBtagEmulationChain::msg() const { return m_msg; }
MsgStream& TrigBtagEmulationChain::msg( const MSG::Level lvl ) const { return msg() << lvl; }

void TrigBtagEmulationChain::addDecisionIngredient(const std::string& decision) { m_ingredientsDecision.push_back(decision); }

bool TrigBtagEmulationChain::parseChainName( const std::string& input ) {
  if ( input.find("HLT_") != std::string::npos ) processHLTtrigger( m_definition,input );
  else if ( input.find("L1_") != std::string::npos ) processL1trigger( m_definition,input );
  else m_correctlyConfigured = false;
  return true;
}

bool TrigBtagEmulationChain::processL1trigger ( std::vector< std::vector< std::string > > &definition, const std::string& triggerName ) {
  std::string input = triggerName;
  
  while ( !input.empty() ) {
    std::string subString = "";

    bool hasSeparation = input.find("_") != std::string::npos;
    subString = hasSeparation ? input.substr( 0,input.find("_") ) : input;
    input = hasSeparation ? input.substr( input.find("_") + 1 , input.length() - input.find("_") -1 ) : "";

    if ( subString == "L1" ) continue;
    if ( definition.size() == 0 ) definition.push_back( std::vector< std::string >() );
    definition.at( definition.size() - 1 ).push_back( "EMUL_L1_" + subString );
  }

  return true;
}

bool TrigBtagEmulationChain::processHLTtrigger ( std::vector< std::vector< std::string > > &definition, const std::string& triggerName ) {

  std::string L1component  = "";
  std::string HLTcomponent = "";

  if ( triggerName.find("L1") != std::string::npos ) {
    L1component  = triggerName.substr( triggerName.find("L1") , triggerName.length() - triggerName.find("L1") );
    HLTcomponent = triggerName.substr( 0 , triggerName.find("L1") - 1);
    L1component.replace( L1component.find("L1") + 2, 0 ,"_" );
  } else HLTcomponent = triggerName;

  // Deal with L1 Components
  std::vector< std::vector< std::string > > definition_L1;
  definition_L1.push_back( std::vector< std::string >() );
  if ( not L1component.empty() ) processL1trigger( definition_L1,L1component );

  // Deal with HLT Components
  std::vector< std::vector< std::string > > definition_HLT;
  definition_HLT.push_back( std::vector< std::string >() );
  
  ///////
  std::string input = HLTcomponent;

  std::vector< std::string > HLTchainComponents;

  while ( !input.empty() ) {
    std::string subString = "";

    bool hasSeparation = input.find("_")!=std::string::npos;
    subString = hasSeparation ? input.substr( 0,input.find("_") ) : input;
    input = hasSeparation ? input.substr( input.find("_") + 1 , input.length() - input.find("_") -1 ) : "";

    if (subString == "HLT") continue;
    else if ( HLTchainComponents.size() == 0 ) HLTchainComponents.push_back( "EMUL_HLT_" + subString );
    else if ( subString.find("j") != std::string::npos ) HLTchainComponents.push_back( "EMUL_HLT_" + subString );
    else if ( subString.find("invm") != std::string::npos )  HLTchainComponents.push_back( "EMUL_HLT_" + subString );
    else HLTchainComponents.at( HLTchainComponents.size() - 1 ) += "_" + subString; 
  }

  std::vector< std::unique_ptr< TrigBtagEmulationChainJetIngredient_HLT > > storedChains;
  for ( const std::string &name : HLTchainComponents ) {
    if ( name.find("gsc")!=std::string::npos )
      storedChains.push_back( std::unique_ptr< TrigBtagEmulationChainJetIngredient_HLT >( new TrigBtagEmulationChainJetIngredient_GSC( msg(),name ) ) );
    else
      storedChains.push_back( std::unique_ptr< TrigBtagEmulationChainJetIngredient_HLT >( new TrigBtagEmulationChainJetIngredient_HLT( msg(),name ) ) );
  }

  for (unsigned int i(0); i<storedChains.size(); i++)
    storedChains.at(i)->initialize();

  // Resolve Conflicts
  std::vector< std::string > conflictingChains;
  int nMismatches = 0;

  ////////

  for ( unsigned int i(0); i<storedChains.size(); i++ ) {
    std::unique_ptr< TrigBtagEmulationChainJetIngredient_HLT > &chain_A =  storedChains.at(i);
    for ( unsigned int j(i+1); j<storedChains.size(); j++ ) {
      std::unique_ptr< TrigBtagEmulationChainJetIngredient_HLT > &chain_B =  storedChains.at(j);

      if ( chain_A->contains( *chain_B.get() ) ) continue;
      if ( chain_B->contains( *chain_A.get() ) ) continue;
      
      if ( chain_A->overlaps( *chain_B.get() ) ) {
	std::vector< std::vector< std::string > > resolvedConflict = chain_A->resolveConflict( *chain_B.get() );
	if ( resolvedConflict.size() == 0 )  m_correctlyConfigured = false; 
	
	conflictingChains.push_back( chain_A->getName() );
	conflictingChains.push_back( chain_B->getName() );

	if ( definition_HLT.size() == 0 ) definition_HLT.push_back( std::vector< std::string >() );
	if ( nMismatches != 0 ) m_correctlyConfigured = false;
	
	for ( const std::string &name : resolvedConflict.at(0) )
	  definition_HLT.at( definition_HLT.size() - 1 ).push_back( name );
	definition_HLT.push_back( std::vector< std::string >() );
	for ( const std::string &name : resolvedConflict.at(1) )
	  definition_HLT.at( definition_HLT.size() - 1 ).push_back( name );

	nMismatches++;
      }
      
    }
  }

  /////// Deal With Multeplicity
  for ( unsigned int i(0); i<storedChains.size(); i++ ) {
    std::unique_ptr< TrigBtagEmulationChainJetIngredient_HLT > &chain_A =  storedChains.at(i);

    // Check chain_A had a conflict 
    bool hasConflict_A = false;
    for ( const std::string &name : conflictingChains ) 
      if ( chain_A->getName() == name ) hasConflict_A = true;
    if ( hasConflict_A ) continue;

    for ( unsigned int j(i+1); j<storedChains.size(); j++ ) {
      std::unique_ptr< TrigBtagEmulationChainJetIngredient_HLT > &chain_B =  storedChains.at(j);

      // Check chain_B had a conflict 
      bool hasConflict_B = false;
      for ( const std::string &name : conflictingChains ) 
	if ( chain_B->getName() == name ) hasConflict_B = true;
      if ( hasConflict_B ) continue;

      if ( chain_A->contains( *chain_B.get() ) ) chain_A->addJetsMulteplicity( *chain_B.get() );
      else if ( chain_B->contains( *chain_A.get() ) ) chain_B->addJetsMulteplicity( *chain_A.get() );
      else if ( chain_A->overlaps( *chain_B.get() ) ) m_correctlyConfigured = false;
    }
    
    for ( unsigned int k(0); k<definition_HLT.size(); k++ )
      definition_HLT.at( k ).push_back( chain_A->getName() );

    // Store Chain
    recordChain( std::move( chain_A ) );
  }

  // Add L1 Configuration to all HLT AND sequences
  for ( const std::vector< std::string >& L1list : definition_L1 ) {
    for ( std::vector< std::string >& HLTlist : definition_HLT ) {
      HLTlist.insert( HLTlist.begin(),L1list.begin(),L1list.end() );
    }
  }

  if ( definition.size() == 1 && definition.at(0).size() == 0)
    definition = definition_HLT;
  else if ( definition_HLT.size() == 1 )
    for ( const std::string& el : definition_HLT.at(0) )
      definition.at( definition.size() - 1 ).push_back( el );
  return true;
}

bool TrigBtagEmulationChain::isCorrectlyConfigured() const { return m_correctlyConfigured; } 
bool TrigBtagEmulationChain::isAutoConfigured() const { return m_autoConfigured; }

bool TrigBtagEmulationChain::isPassed() {

  for ( unsigned int i(0); i<m_definition.size(); i++ ) {
    bool fires = true;

    for ( unsigned int j(0); j<m_definition.at(i).size(); j++ )
      if ( not isPassed( m_definition.at(i).at(j) ) ) fires = false;

    if ( fires ) return true;
  }

  return false;
}

bool TrigBtagEmulationChain::isPassed( const std::string &chain ) const {
  if ( chain.find( "EMUL" ) == std::string::npos )
    return m_trigDec->isPassed( chain.c_str() );

  if ( m_chains.find( chain ) != m_chains.end() ) 
    return m_chains.at( chain.c_str() )->isPassed();
  return false;
}

void TrigBtagEmulationChain::print() {
  ATH_MSG_INFO( "" );
  ATH_MSG_INFO( "EMULATING TRIGGER : " << m_name );

  for ( unsigned int i(0); i < m_definition.size(); i++ ) {
    if ( i != 0 ) ATH_MSG_INFO( " __ OR __ " );
    for ( unsigned int j(0); j<m_definition.at(i).size(); j++ ) 
      ATH_MSG_INFO( "  AND : " << m_definition.at(i).at(j) );
  }
  
  ATH_MSG_INFO( "### TRIGGER EMULATION ###" );
  for ( unsigned int i(0); i < m_definition.size(); i++ )
    for ( unsigned int j(0); j < m_definition.at(i).size(); j++ )
      if ( m_definition.at(i).at(j).find("EMUL") != std::string::npos ) m_chains.at( m_definition.at(i).at(j) )->print();
      else ATH_MSG_INFO( "### TDT Outcome for Trigger " << m_definition.at(i).at(j) << " : " << m_trigDec->isPassed( m_definition.at(i).at(j).c_str() ) );
}

bool TrigBtagEmulationChain::hasFeature(const std::string& feature) {
  for ( unsigned int i(0); i < m_definition.size(); i++ )
    for ( unsigned int j(0); j < m_definition.at(i).size(); j++ )
      if ( m_chains.at( m_definition.at(i).at(j) )->hasFeature( feature ) ) 
	return true;
  return false;
}

void TrigBtagEmulationChain::evaluate() {
  if ( TrigBtagEmulationChain::hasBeenEvaluated == true ) return;

  std::map< std::string, std::unique_ptr< BaseTrigBtagEmulationChainJetIngredient > >::iterator it = m_chains.begin();
  for ( ; it != m_chains.end(); it++ )
    it->second->evaluate();

  TrigBtagEmulationChain::hasBeenCleared   = false;
  TrigBtagEmulationChain::hasBeenEvaluated = true;
}

void TrigBtagEmulationChain::clear() {
  if ( TrigBtagEmulationChain::hasBeenCleared == true ) return;

  std::map< std::string, std::unique_ptr< BaseTrigBtagEmulationChainJetIngredient > >::iterator it = m_chains.begin();
  for ( ; it != m_chains.end(); it++ )
    it->second->clear();

  TrigBtagEmulationChain::hasBeenCleared   = true;
  TrigBtagEmulationChain::hasBeenEvaluated = false;
}


//**********************************************************************  

bool TrigBtagEmulationChain::recordChain() {
  for ( unsigned int i(0); i < m_definition.size(); i++ )
    for ( unsigned int j(0); j < m_definition.at(i).size(); j++ )
      if ( m_definition.at(i).at(j).find("EMUL")!=std::string::npos ) recordChain( m_definition.at(i).at(j) );
      else addDecisionIngredient( m_definition.at(i).at(j) );
  return true;
}

bool TrigBtagEmulationChain::recordChain( const std::string& chainName ) {
  if ( m_chains.find( chainName ) != m_chains.end() ) return true;

  std::unique_ptr< BaseTrigBtagEmulationChainJetIngredient > toBeAdded;
  if ( chainName.find( "gsc" ) != std::string::npos ) 
    toBeAdded = std::unique_ptr< BaseTrigBtagEmulationChainJetIngredient >( new TrigBtagEmulationChainJetIngredient_GSC( msg(),chainName ) );
  else if ( chainName.find( "HLT" ) != std::string::npos ) 
    toBeAdded = std::unique_ptr< BaseTrigBtagEmulationChainJetIngredient >( new TrigBtagEmulationChainJetIngredient_HLT( msg(),chainName ) );
  else if ( chainName.find( "MJJ" ) == std::string::npos && chainName.find( "JJ" ) != std::string::npos ) 
    toBeAdded = std::unique_ptr< BaseTrigBtagEmulationChainJetIngredient >( new TrigBtagEmulationChainJetIngredient_L1_JJ( msg(),chainName ) );
  else
    toBeAdded = std::unique_ptr< BaseTrigBtagEmulationChainJetIngredient >( new TrigBtagEmulationChainJetIngredient_L1( msg(),chainName ) );

  toBeAdded->initialize();
  recordChain( std::move(toBeAdded) );

  return true;
}

bool TrigBtagEmulationChain::recordChain( std::unique_ptr< BaseTrigBtagEmulationChainJetIngredient > chain ) {
  const std::string chainName = chain->getName();

  if ( m_chains.find( chainName ) != m_chains.end() ) return true;
  m_chains[ chainName.c_str() ] = std::move( chain );

  return true;
}

//**********************************************************************

