/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration 
*/

/**********************************************************************
 * Authors:
 *      Carlo Varni <carlo.varni@cern.ch>
 *      Carlo Schiavi <carlo.schiavi@cern.ch>
 *      Florencia Daneri <maria.florencia.daneri@cern.ch>
 *      Gino Marceca <gino.marceca@cern.ch>
 *      Lars Beemster <lars.beemster@cern.ch>
 *
 * Description:
 *      Base tool class for bjet trigger emulation
 **********************************************************************/

#include "src/TrigBtagEmulationTool.h"

namespace Trig {

TrigBtagEmulationTool::TrigBtagEmulationTool(const std::string& type, 
					     const std::string& name, 
					     const IInterface* parent)
  : base_class(type, name, parent) 
{}


StatusCode TrigBtagEmulationTool::initialize() {
  ATH_MSG_INFO( "Initializing " << name() );

  ATH_CHECK( m_trigDec.retrieve() );

  ATH_CHECK( m_manager_EMTopo_cnt.retrieve() );
  ATH_CHECK( m_manager_PFlow_cnt.retrieve() );
  
  ATH_CHECK( m_manager_EMTopo_presel.retrieve() );
  
  for(const auto& [chain_name, definition] : m_emulatedChainDefinitions) {
    ATH_CHECK( addEmulatedChain(chain_name, definition) );
  }
  
  return StatusCode::SUCCESS;
}

bool TrigBtagEmulationTool::isPassed(const std::string& chain) const 
{
  std::unordered_map< std::string, std::unique_ptr< TrigBtagEmulationChain >>::const_iterator itr = m_emulatedChains.find(chain);
  if (itr == m_emulatedChains.end()) {
    ATH_MSG_WARNING( "chain " << chain << " requested but not in the list of emulated chains" );
    return false;
  }
  const std::unique_ptr< TrigBtagEmulationChain >& obj = itr->second;
  return obj->isPassed();
}


//=================================================
//		Utility functions
//=================================================
StatusCode TrigBtagEmulationTool::populateJetManagersTriggerObjects() {
  // Check if input chain exists
  ATH_CHECK( checkInputChainExists( m_inputChains_PFlow ) );
  
  // Retrieve input container vectors
  ATH_CHECK( retrieveTriggerObjects( m_manager_EMTopo_cnt, /*useTriggerNavigation=*/false ) );
  ATH_CHECK( retrieveTriggerObjects( m_manager_PFlow_cnt, /*useTriggerNavigation=*/false ) );
    
  ATH_CHECK( retrieveTriggerObjects( m_manager_EMTopo_presel, /*useTriggerNavigation=*/false ) );
  
  return StatusCode::SUCCESS;
}

StatusCode TrigBtagEmulationTool::retrieveTriggerObjects( ToolHandle< Trig::JetManagerTool >& manager, 
							  bool useTriggerNavigation)
{
  StatusCode sc = useTriggerNavigation 
    ? manager->retrieveByNavigation()
    : manager->retrieveByContainer();
  
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Could not retrieve Jets!" );
    return StatusCode::FAILURE;
  }
  
  const std::string& chainName = manager->chainName();
  const std::string& jetContainerName = manager->jetContainerName();
  
  const unsigned int jetSize = manager->jetSize();
  const unsigned int jetRoISize = manager->jetRoISize();
  const unsigned int btaggingSize= manager->btaggingSize();
  
  if (useTriggerNavigation) {
    ATH_MSG_DEBUG( "Size of input containers ['" << chainName << "'] :"
		   << " jet=" << jetSize
		   << " jetRoI=" << jetRoISize
		   << " btag=" << btaggingSize);
  }
  else {
    ATH_MSG_DEBUG( "Size of input containers ['" << jetContainerName << "'] :"
		   << " jet=" << jetSize
		   << " jetRoI=" << jetRoISize);
  }
  
  return StatusCode::SUCCESS;
}

StatusCode TrigBtagEmulationTool::checkInputChainExists( const std::string &inputChain ) const 
{
  std::vector<std::string> listOfTriggers = m_trigDec->getChainGroup( inputChain )->getListOfTriggers();
  if ( listOfTriggers.empty() ) {
    ATH_MSG_ERROR( "Input Chain ('" << inputChain << "') is not in the list of available triggers: {" << m_trigDec->getListOfTriggers(".*") << "}" );
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_INFO( "Triggers in configured ChainGroup " << inputChain << ":" );
  for (const auto& cname: listOfTriggers) {
    ATH_MSG_INFO( " └ " << cname );
  }
  
  return StatusCode::SUCCESS;
}

StatusCode TrigBtagEmulationTool::addEmulatedChain(const std::string& triggerName, 
						   const std::vector<std::string>& definition) 
{
  std::unique_ptr< TrigBtagEmulationChain > chain = std::make_unique<TrigBtagEmulationChain>( triggerName, definition, m_trigDec );
  chain->setLevel(msgLevel());

  ATH_CHECK( chain->addJetManager(m_manager_EMTopo_cnt.name(), m_manager_EMTopo_cnt) );
  ATH_CHECK( chain->addJetManager(m_manager_PFlow_cnt.name(), m_manager_PFlow_cnt) );
  ATH_CHECK( chain->addJetManager(m_manager_EMTopo_presel.name(), m_manager_EMTopo_presel) );
  
  m_emulatedChains.insert( std::make_pair(triggerName, std::move(chain)) );
  
  return StatusCode::SUCCESS;
}

}
