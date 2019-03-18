/*
Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration 
*/

/**********************************************************************
 * AsgTool: TrigBtagEmulationTool
 *
 * Authors:
 *      Carlo Varni <carlo.varni@cern.ch>
 *      Carlo Schiavi <carlo.schiavi@cern.ch>
 *      Florencia Daneri <maria.florencia.daneri@cern.ch>
 *      Gino Marceca <gino.marceca@cern.ch>
 *
 * Description:
 *      Base tool class for bjet trigger emulation
 **********************************************************************/

#include "TrigBtagEmulationTool/TrigBtagEmulationTool.h"

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

TrigBtagEmulationTool::TrigBtagEmulationTool( const std::string& name )
  : AsgTool(name),
    m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
#ifdef XAOD_STANDALONE
#elif defined( XAOD_ANALYSIS )
    m_storeGate("StoreGateSvc", name),
#else
    m_storeGate("StoreGateSvc", name),
    m_bTagTool("Analysis::BTagTool"),
    m_bTagTrackAssocTool("Analysis::BTagTrackAssociation"),
    m_bTagSecVtxTool("Analysis::BTagSecVertexing"),
#endif
    m_inputChains( std::make_tuple("","","") ),
    m_jetKeys( std::make_tuple("","","") ),
    m_pvKeys( std::make_tuple("","","") ),
    m_tpKeys( std::make_tuple("","","") ),
    m_jetContainers( std::make_tuple("","","") ),
    m_previousEvent(0),
    m_splitTrigger(false),
    m_btagTrigger(false),
    m_htTrigger(false),
    m_gscTrigger(false)
{

#if !defined( XAOD_STANDALONE ) && !defined( XAOD_ANALYSIS )
  declareProperty("BTagTool",            m_bTagTool);
  declareProperty("BTagTrackAssocTool",  m_bTagTrackAssocTool);
  declareProperty("BTagSecVertexing",    m_bTagSecVtxTool);
#endif

  declareProperty("UseTriggerNavigation", m_useTriggerNavigation=true);
  declareProperty("TagOfflineWeights",    m_tagOfflineWeights=false);
  declareProperty("TagOnlineWeights",     m_tagOnlineWeights=false);

  // Input Chains
  declareProperty("InputChain",                  std::get< jetCollections::EF >( m_inputChains )    = "HLT_j35_boffperf");
  declareProperty("InputChainSplit",             std::get< jetCollections::SPLIT >( m_inputChains ) = "HLT_j35_boffperf_split");
  declareProperty("InputChainGSC",               std::get< jetCollections::GSC >( m_inputChains )   = "HLT_j15_gsc35_boffperf_split");

  // Jet Keys
  declareProperty("InputJetKey",                 std::get< jetCollections::EF >( m_jetKeys )    = "EFJet" );
  declareProperty("InputJetKey_Split",           std::get< jetCollections::SPLIT >( m_jetKeys ) = "SplitJet" );
  declareProperty("InputJetKey_GSC",             std::get< jetCollections::GSC >( m_jetKeys )   = "GSCJet" );

  // Primary Vertex Keys
  declareProperty("InputPrimaryVertexKey",       std::get< jetCollections::EF >( m_pvKeys )    = "EFHistoPrmVtx" );
  declareProperty("InputPrimaryVertexKey_Split", std::get< jetCollections::SPLIT >( m_pvKeys ) = "xPrimVx" );
  declareProperty("InputPrimaryVertexKey_GSC",   std::get< jetCollections::GSC >( m_pvKeys )   = "xPrimVx" );

  // Track Particle Keys
  declareProperty("InputTrackParticleKey",       std::get< jetCollections::EF >( m_tpKeys )    = "" );
  declareProperty("InputTrackParticleKey_Split", std::get< jetCollections::SPLIT >( m_tpKeys ) = "InDetTrigTrackingxAODCnv_Bjet_IDTrig" );
  declareProperty("InputTrackParticleKey_GSC",   std::get< jetCollections::GSC >( m_tpKeys )   = "InDetTrigTrackingxAODCnv_Bjet_IDTrig" );

  // Jet Containers
  declareProperty("InputJetName",                std::get< jetCollections::EF >( m_jetContainers )    = "HLT_xAOD__JetContainer_EFJet");
  declareProperty("InputJetName_Split",          std::get< jetCollections::SPLIT >( m_jetContainers ) = "HLT_xAOD__JetContainer_SplitJet");
  declareProperty("InputJetName_GSC",            std::get< jetCollections::GSC >( m_jetContainers )   = "HLT_xAOD__JetContainer_GSCJet" );

  // Btag Container
  declareProperty("InputBTaggingName",           m_input_btagName="HLT_xAOD__BTaggingContainer_HLTBjetFex");

  declareProperty("TriggerMenu"             ,    m_trigger_menu="v7");

  declareProperty("EmulatedChainDefinitions",    m_emulatedChainDefinitions);
  declareProperty("AutoConfigMenu"          ,    m_autoconfiguredMenu="");

#if defined( XAOD_STANDALONE )
  declareProperty("TrigDecisionToolName", m_TrigDecToolName = "Trig::TrigDecisionTool/TrigDecisionTool");
  declareProperty("xAODConfigToolName"  , m_xAODConfToolName = "TrigConf::xAODConfigTool");
#endif
}

TrigBtagEmulationTool::~TrigBtagEmulationTool() {}

//**********************************************************************

StatusCode TrigBtagEmulationTool::initialize() {
  
  ATH_MSG_INFO( "Initialising tool " << name() );

#if !defined( XAOD_STANDALONE )
  // RETRIEVE SERVICES
  ANA_CHECK( retrieveTool( m_storeGate ) );
  ANA_CHECK( retrieveTool( m_trigDec ) );
#else

  if ( asg::ToolStore::contains<Trig::TrigDecisionTool>( m_TrigDecToolName.c_str() ) )
    m_trigDec = asg::ToolStore::get<Trig::TrigDecisionTool>( m_TrigDecToolName.c_str() );
  else
    {
      TrigConf::xAODConfigTool *m_trigConfigTool = 0;
      if (asg::ToolStore::contains<TrigConf::xAODConfigTool>( m_xAODConfToolName.c_str() ) )
	m_trigConfigTool = asg::ToolStore::get<TrigConf::xAODConfigTool>( m_xAODConfToolName.c_str() );
      else 
	{
	  m_trigConfigTool = new TrigConf::xAODConfigTool( m_xAODConfToolName.c_str() );
	  ANA_CHECK( m_trigConfigTool->initialize() );
	}
      ToolHandle< TrigConf::ITrigConfigTool > trigConfigHandle( m_trigConfigTool );
      
      m_trigDec = new Trig::TrigDecisionTool( m_TrigDecToolName.c_str() );
      ANA_CHECK( m_trigDec->setProperty( "ConfigTool", trigConfigHandle ) );
      ANA_CHECK( m_trigDec->setProperty( "TrigDecisionKey", "xTrigDecision" ) );
      ANA_CHECK( m_trigDec->initialize() );
    }

#endif


#if !defined( XAOD_STANDALONE ) && !defined( XAOD_ANALYSIS )
  // RETRIEVE OFFLINE TOOLS
  if (!m_bTagTrackAssocTool.empty()) {
    ANA_CHECK( retrieveTool( m_bTagTrackAssocTool ) );
  } else
    ATH_MSG_DEBUG( "No track association tool to retrieve" );

  ANA_CHECK( retrieveTool( m_bTagSecVtxTool ) );
  ANA_CHECK( retrieveTool( m_bTagTool ) );
#endif


  // Autoconfiguration of Trigger Menu
  if (this->initTriggerChainsMenu().isFailure() ) {
    ATH_MSG_ERROR( "Could not initialize lowest unprescaled Trigger Chains!" );
    return StatusCode::FAILURE;
  }

  if ( m_autoconfiguredMenu.find("2015") != std::string::npos || 
       m_autoconfiguredMenu.find("2016") != std::string::npos ||
       m_autoconfiguredMenu.find("2017") != std::string::npos || 
       m_autoconfiguredMenu.find("2018") != std::string::npos ) {
    ATH_MSG_INFO( "Automatic configuration of Trigger Chains for " << m_autoconfiguredMenu );
    ATH_MSG_INFO( "For full list of trigger chains automatically loaded look here : 'https://twiki.cern.ch/twiki/bin/view/Atlas/TrigBjetEmulation'" );
    this->addEmulatedChain( m_autoconfiguredMenu );
  }
  
  // CREATE EMULATED CHAINS
  for (auto definition : m_emulatedChainDefinitions) {
    checkTriggerChain(definition);

    std::vector< std::string > chainDefinition( definition.begin(), definition.end() );
    // Check if trigger is already stored as a lower-unprescaled trigger
    if (chainDefinition.size() == 1) {
      std::string triggerSubComponents = "";
      if ( std::get< TriggerMenu::YEAR_2015 >( m_triggerMenus ).find( chainDefinition.at(0) ) != std::get< TriggerMenu::YEAR_2015 >( m_triggerMenus ).end() )
	triggerSubComponents = std::get< TriggerMenu::YEAR_2015 >( m_triggerMenus ).at( chainDefinition.at(0) );
      else if ( std::get< TriggerMenu::YEAR_2016 >( m_triggerMenus ).find( chainDefinition.at(0) ) != std::get< TriggerMenu::YEAR_2016 >( m_triggerMenus ).end() )
	triggerSubComponents = std::get< TriggerMenu::YEAR_2016 >( m_triggerMenus ).at( chainDefinition.at(0) );
      else if ( std::get< TriggerMenu::YEAR_2017 >( m_triggerMenus ).find( chainDefinition.at(0) ) != std::get< TriggerMenu::YEAR_2017 >( m_triggerMenus ).end() )
	triggerSubComponents = std::get< TriggerMenu::YEAR_2017 >( m_triggerMenus ).at( chainDefinition.at(0) );
      else if ( std::get< TriggerMenu::YEAR_2018 >( m_triggerMenus ).find( chainDefinition.at(0) ) != std::get< TriggerMenu::YEAR_2018 >( m_triggerMenus ).end() )
	triggerSubComponents = std::get< TriggerMenu::YEAR_2018 >( m_triggerMenus ).at( chainDefinition.at(0) );

      std::istringstream sname( triggerSubComponents );
      std::string token;
      while(std::getline( sname , token, ','))
	chainDefinition.push_back( Form("EMUL_%s",token.c_str()) );
    }

    // We have to massage the definition so that it is accepted by the TrigBtagEmulationChain class
    std::vector< std::vector< std::string > > logicalDefinition(1);
    for ( unsigned int i(1); i < chainDefinition.size(); i++ ) {
      std::string chainComponent = chainDefinition.at(i);
      if ( chainComponent == "AND" ) continue;
      if ( chainComponent == "OR" ) {
	logicalDefinition.push_back( std::vector< std::string >() );
	continue;
      }
      logicalDefinition.at( logicalDefinition.size() - 1 ).push_back( chainComponent );
    }

    std::unique_ptr< TrigBtagEmulationChain > chain;
    if ( chainDefinition.size() == 1 ) chain = std::unique_ptr< TrigBtagEmulationChain >( new TrigBtagEmulationChain( msg(),chainDefinition.at(0),m_trigDec ) );
    else chain = std::unique_ptr< TrigBtagEmulationChain >( new TrigBtagEmulationChain( msg(),chainDefinition.at(0),logicalDefinition,m_trigDec ) );

    if ( chain->isAutoConfigured() ) {
      ATH_MSG_INFO( "AUTOMATIC PARSER has been activated for trigger chain : " << chain->name() );

      std::vector< std::vector< std::string > > subChains = chain->definition();
      ATH_MSG_INFO("This trigger has been configured as follows : ");
      for (unsigned int i(0); i<subChains.size(); i++) { 
	if ( i != 0 ) ATH_MSG_INFO("  __OR__ ");
	for (unsigned int j(0); j<subChains.at(i).size(); j++) 
	  ATH_MSG_INFO( "  -- AND :: " << subChains.at(i).at(j) );
      }

    }
    if ( !chain->isCorrectlyConfigured() ) {
      ATH_MSG_FATAL("NOT correct Configuration for Trigger Chain : " << chain->name() << ".");
      return StatusCode::FAILURE;
    }

    std::string chainNameToBeSaved = chain->name();
    m_emulatedChains.insert( std::make_pair( chainNameToBeSaved,*chain.release() ) );
  }

  // Jet Managers
  // Set manager for LVL1 jets 
  m_manager_lvl1_8x8  = std::unique_ptr< Trig::JetManager >( new Trig::JetManager( "TrigBtagEmulationTool::JM_LVL1_8x8",m_trigDec, "" ) );
  m_manager_lvl1_4x4  = std::unique_ptr< Trig::JetManager >( new Trig::JetManager( "TrigBtagEmulationTool::JM_LVL1_4x4",m_trigDec, "" ) );
  m_manager_lvl1_8x8->setContainers( "LVL1JetRoIs","" );
  m_manager_lvl1_4x4->setContainers( "LVL1JetRoIs","" );
  m_manager_lvl1_4x4->use4x4( true );

  // Set the manager for raw Jets (used for HT)
  // Set the m+chain since we skip the event in case we do not posses a source of unbiased events. Thus, this must be done AFTER the loading of chains.
  m_manager_HT        = std::unique_ptr< Trig::JetManager >( new Trig::JetManager( "TrigBtagEmulationTool::JM_HT",m_trigDec, hasGSC() ? std::get< jetCollections::GSC >( m_inputChains ) : std::get< jetCollections::SPLIT >( m_inputChains ) ) ); 
  m_manager_HT->setContainers( m_trigger_menu.find("v7")!=std::string::npos ? "HLT_xAOD__JetContainer_a4tcemsubjesISFS" : "HLT_xAOD__JetContainer_a4tcemsubjesFS" , "" );

  // Set the HLT jets
  m_manager_ef        = std::unique_ptr< Trig::JetManager >( new Trig::JetManager( "TrigBtagEmulationTool::JM_EF",m_trigDec, std::get< jetCollections::EF >( m_inputChains ) ) );
  m_manager_split     = std::unique_ptr< Trig::JetManager >( new Trig::JetManager( "TrigBtagEmulationTool::JM_SPLIT",m_trigDec, std::get< jetCollections::SPLIT >( m_inputChains ) ) );
  m_manager_gsc       = std::unique_ptr< Trig::JetManager >( new Trig::JetManager( "TrigBtagEmulationTool::JM_GSC",m_trigDec, std::get< jetCollections::GSC >( m_inputChains ) ) );
  m_manager_split_gsc = std::unique_ptr< Trig::JetManager >( new Trig::JetManager( "TrigBtagEmulationTool::JM_SPLIT_GSC",m_trigDec, std::get< jetCollections::GSC >( m_inputChains ) ) );
  // Set The Container names
  m_manager_ef->setContainers       ( std::get< jetCollections::EF >( m_jetContainers )   ,m_input_btagName);
  m_manager_split->setContainers    ( std::get< jetCollections::SPLIT >( m_jetContainers ),m_input_btagName);
  m_manager_gsc->setContainers      ( std::get< jetCollections::GSC >( m_jetContainers )  ,m_input_btagName);
  m_manager_split_gsc->setContainers( std::get< jetCollections::GSC >( m_jetContainers )  ,m_input_btagName );
  // Set The Keys for the Trigger Navigation
  m_manager_ef->setKeys       ( std::get< jetCollections::EF >( m_jetKeys ),
				"HLTBjetFex",
				std::get< jetCollections::EF >( m_pvKeys ),
				std::get< jetCollections::EF >( m_tpKeys ) );
  m_manager_split->setKeys    ( std::get< jetCollections::SPLIT >( m_jetKeys ),
				"HLTBjetFex",
                                std::get< jetCollections::SPLIT >( m_pvKeys ),
                                std::get< jetCollections::SPLIT >( m_tpKeys ) );
  m_manager_gsc->setKeys      ( std::get< jetCollections::GSC >( m_jetKeys ),
				"HLTBjetFex",
                                std::get< jetCollections::GSC >( m_pvKeys ),
                                std::get< jetCollections::GSC >( m_tpKeys ) );
  m_manager_split_gsc->setKeys( std::get< jetCollections::SPLIT >( m_jetKeys ),
				"HLTBjetFex",
                                std::get< jetCollections::SPLIT >( m_pvKeys ),
                                std::get< jetCollections::SPLIT >( m_tpKeys ) );


#if !defined( XAOD_STANDALONE ) && !defined( XAOD_ANALYSIS )
  JetManager::m_bTagTool = &m_bTagTool;
  JetManager::m_bTagTrackAssocTool = &m_bTagTrackAssocTool;
  JetManager::m_bTagSecVtxTool = &m_bTagSecVtxTool;
#endif

  return StatusCode::SUCCESS;
}


//!==========================================================================
StatusCode TrigBtagEmulationTool::execute() {

  ATH_MSG_DEBUG( "Executing tool " << name() );

  // CLEAR PREVIOUS RESULTS
  clear();

  // CHECK INPUT CHAINS EXISTS
  if ( not hasGSC() ) {
    ANA_CHECK( checkInputChainExists( std::get< jetCollections::SPLIT >( m_inputChains ) ) );
  } else {
    ANA_CHECK( checkInputChainExists( std::get< jetCollections::GSC >( m_inputChains ) ) );
  }

  // RETRIEVE INPUT CONTAINER VECTORS
  // RETRIEVE LVL1 JETROIs
  ANA_CHECK( retrieve( m_manager_lvl1_8x8,false ) );
  ANA_CHECK( retrieve( m_manager_lvl1_4x4,false ) );

  // Retrieve raw jets for HT computation
  ANA_CHECK( retrieve( m_manager_HT , false ) );

  // Retrieve HLT Jets
  ANA_CHECK( retrieve( m_manager_ef   ,m_useTriggerNavigation ) );
  ANA_CHECK( retrieve( m_manager_split,m_useTriggerNavigation ) );
  if ( this->hasGSC() ) ANA_CHECK( retrieve( m_manager_gsc       ,m_useTriggerNavigation ) );
  if ( this->hasGSC() ) ANA_CHECK( retrieve( m_manager_split_gsc ,m_useTriggerNavigation ) );

  // RETAG COPYING ORIGINAL WEIGHTS 
  if (!m_useTriggerNavigation || !m_tagOfflineWeights || !m_tagOnlineWeights) {
    ANA_CHECK( m_manager_ef->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
    ANA_CHECK( m_manager_split->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
    if ( this->hasGSC() ) ANA_CHECK( m_manager_gsc->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
    if ( this->hasGSC() ) ANA_CHECK( m_manager_split_gsc->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
  }

  // RETAG WITH OFFLINE TOOLS    
  if (m_useTriggerNavigation && m_tagOfflineWeights) {
    ANA_CHECK( m_manager_ef->retagOffline()        );
    ANA_CHECK( m_manager_split->retagOffline()     );
    if ( this->hasGSC() ) ANA_CHECK( m_manager_gsc->retagOffline()       );
    if ( this->hasGSC() ) ANA_CHECK( m_manager_split_gsc->retagOffline() );
  }

  // RETAG WITH ONLINE TOOLS
  if (m_useTriggerNavigation && m_tagOnlineWeights) {
    ANA_CHECK( m_manager_ef->retagOnline()        );
    ANA_CHECK( m_manager_split->retagOnline()     );
    if ( this->hasGSC() ) ANA_CHECK( m_manager_gsc->retagOnline()       );
    if ( this->hasGSC() ) ANA_CHECK( m_manager_split_gsc->retagOnline() );
  }
  
  // BACKUP NON-CENTRAL JET INFO FOR SPLIT CHAINS  
  m_manager_ef->merge( m_manager_split );
  m_manager_split->merge( m_manager_ef );
  m_manager_ef->merge( m_manager_HT );
  m_manager_split->merge( m_manager_HT );

  if ( this->hasGSC() ) m_manager_split_gsc->merge( m_manager_HT,15 * 1e3 ); 


  // Clearing jets
  BaseTrigBtagEmulationChainJetIngredient::clearJet();
  // Saving Jets
  BaseTrigBtagEmulationChainJetIngredient::addJet( "LVL1"      ,m_manager_lvl1_8x8->getJets()  );
  BaseTrigBtagEmulationChainJetIngredient::addJet( "LVL1_JJ"   ,m_manager_lvl1_4x4->getJets()  );
  BaseTrigBtagEmulationChainJetIngredient::addJet( "EF"        ,m_manager_ef->getJets()        );
  BaseTrigBtagEmulationChainJetIngredient::addJet( "SPLIT"     ,m_manager_split->getJets()     );
  if ( this->hasGSC() ) BaseTrigBtagEmulationChainJetIngredient::addJet( "GSC"       ,m_manager_gsc->getJets()       );
  if ( this->hasGSC() ) BaseTrigBtagEmulationChainJetIngredient::addJet( "SPLIT_GSC" ,m_manager_split_gsc->getJets() );

  // EVALUATE EMULATED CHAINS
  TrigBtagEmulationChain::clear();
  TrigBtagEmulationChain::evaluate();

  // DUMP L1 JETS
  ATH_MSG_DEBUG ("L1 jets");
  
  std::vector< std::unique_ptr< TrigBtagEmulationJet > > l1_jets = m_manager_lvl1_8x8->getJets();  
  std::vector< std::unique_ptr< TrigBtagEmulationJet > > l1_jets_jj = m_manager_lvl1_4x4->getJets();
  
  for (unsigned int i=0; i<l1_jets.size(); i++) {
    auto & l1_jet    = l1_jets.at(i);
    auto & l1_jet_jj = l1_jets_jj.at(i);
    ATH_MSG_DEBUG ("  Jet --- pt[et8x8]=" << l1_jet->pt() << " pt[et4x4]="<< l1_jet_jj->pt()  << " eta=" << l1_jet->eta() << " phi=" << l1_jet->phi() );
  }
    
    // DUMP EF JETS
  ATH_MSG_DEBUG ("EF jets");
  for (std::unique_ptr< TrigBtagEmulationJet >& jet : m_manager_ef->getJets()) {
    ATH_MSG_DEBUG ("  Jet --- pt=" << jet->pt()*1e-3 << " eta=" << jet->eta() << " phi=" << jet->phi());
    for (auto & weight : jet->weights()) 
      ATH_MSG_DEBUG ("      " << weight.first << " " << weight.second);
  }
    
  // DUMP SPLIT JETS
  ATH_MSG_DEBUG ("SPLIT jets");
  for (std::unique_ptr< TrigBtagEmulationJet >& jet : m_manager_split->getJets()) {
    if ( jet->pt()*1e-3 < 15 ) continue;
    ATH_MSG_DEBUG ("  Jet --- pt=" << jet->pt()*1e-3 << " eta=" << jet->eta() << " phi=" << jet->phi());
    for (auto & weight : jet->weights())
      ATH_MSG_DEBUG ("      " << weight.first << " " << weight.second);
  }
  
  // DUMP GSC JETS
  ATH_MSG_DEBUG ("GSC jets");
  for (std::unique_ptr< TrigBtagEmulationJet >& jet : m_manager_gsc->getJets()) {
    if ( jet->pt()*1e-3 < 15 ) continue;
    ATH_MSG_DEBUG ("  Jet --- pt=" << jet->pt()*1e-3 << " eta=" << jet->eta());
    for (auto & weight : jet->weights())
      ATH_MSG_DEBUG ("      " << weight.first << " " << weight.second);
  }

  // DUMP EMULATED DECISIONS
  ATH_MSG_DEBUG ("Emulated decisions");
  for (auto& emulatedChain : m_emulatedChains) {
    emulatedChain.second.print(); 
    ATH_MSG_DEBUG ("  Chain --- name=" << emulatedChain.first << " result=" << (int)emulatedChain.second.isPassed());
  }
  
  return StatusCode::SUCCESS;
}


//!==========================================================================
StatusCode TrigBtagEmulationTool::finalize() {
  ATH_MSG_INFO( "Finalising tool " << name() );
  BaseTrigBtagEmulationChainJetIngredient::clearJet();
  return StatusCode::SUCCESS;
}


//!==========================================================================
StatusCode TrigBtagEmulationTool::addEmulatedChain(const std::vector<std::string>& chainDescription) {
  // check if gsc chains are present in the emulation
  checkTriggerChain(chainDescription);

  // Create chain and add it to the list
  std::string triggerName = chainDescription.at(0);
  std::vector< std::vector< std::string > > defToBeAdded(1);
  for ( unsigned int i(1); i<chainDescription.size(); i++ ) {
    std::string chainComponent = chainDescription.at(i);
    if ( chainComponent == "AND" ) continue;
    if ( chainComponent == "OR" ) {
      defToBeAdded.push_back( std::vector< std::string >() );
      continue;
    }
    defToBeAdded.at( defToBeAdded.size() - 1 ).push_back( chainComponent );
  }

  std::unique_ptr< TrigBtagEmulationChain > chain;
  if ( chainDescription.size() == 1 ) chain = std::unique_ptr< TrigBtagEmulationChain >( new TrigBtagEmulationChain( msg(),triggerName,m_trigDec ) );
  else chain = std::unique_ptr< TrigBtagEmulationChain >( new TrigBtagEmulationChain( msg(),triggerName,defToBeAdded,m_trigDec ) );

  // Check it has been auto configured
  if ( chain->isAutoConfigured() ) {
    ATH_MSG_INFO( "AUTOMATIC PARSER has been activated for trigger chain : " << chain->name() );
    std::vector< std::vector< std::string > > subChains = chain->definition();
    ATH_MSG_INFO("This trigger has been configured as follows : ");
    for (unsigned int i(0); i<subChains.size(); i++) {
      if ( i != 0 ) ATH_MSG_INFO("  __OR__ ");
      for (unsigned int j(0); j<subChains.at(i).size(); j++)
	ATH_MSG_INFO( "  -- AND :: " << subChains.at(i).at(j) );
    }
  }
  // Check it has been conrrectyle configured
  if ( not chain->isCorrectlyConfigured() ) {
    ATH_MSG_FATAL("NOT correct Configuration for Trigger Chain : " << chain->name() << "." );
    return StatusCode::FAILURE;
  }

  std::string chainNameToBeSaved = chain->name();
  m_emulatedChains.insert( std::make_pair( chainNameToBeSaved,*chain.release() ) );
  return StatusCode::SUCCESS;
}

//!==========================================================================        

StatusCode TrigBtagEmulationTool::initTriggerChainsMenu() {
  if ( initTriggerChainsMenu( 2015 ).isFailure() ) return StatusCode::FAILURE;
  if ( initTriggerChainsMenu( 2016 ).isFailure() ) return StatusCode::FAILURE;
  if ( initTriggerChainsMenu( 2017 ).isFailure() ) return StatusCode::FAILURE;
  if ( initTriggerChainsMenu( 2018 ).isFailure() ) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

StatusCode TrigBtagEmulationTool::initTriggerChainsMenu(const int year) {

  const std::string fileName = PathResolverFindDataFile( Form( "TrigBtagEmulationTool/triggerChains_%dMenu.txt",year) );
  std::ifstream file; file.open( fileName.c_str() );
  if (!file) {
    ATH_MSG_ERROR( "Could not load lowest unprescaled Trigger Chains for " << year << " ! " );
    return StatusCode::FAILURE;
  }

  while ( !file.eof() ) {
    std::string line;
    getline (file,line);
    if (line.empty()) continue;
    std::istringstream is_line(line);
    std::string chainName, chainComponents;
    is_line >> chainName >> chainComponents;

    if (year == 2015) 
      std::get< TriggerMenu::YEAR_2015 >( m_triggerMenus ).insert( std::make_pair(chainName,chainComponents) );
    else if (year == 2016)
      std::get< TriggerMenu::YEAR_2016 >( m_triggerMenus ).insert( std::make_pair(chainName,chainComponents) );
    else if (year == 2017)
      std::get< TriggerMenu::YEAR_2017 >( m_triggerMenus ).insert( std::make_pair(chainName,chainComponents) );
    else if (year == 2018)
      std::get< TriggerMenu::YEAR_2018 >( m_triggerMenus ).insert( std::make_pair(chainName,chainComponents) );
  }
  file.close();

  return StatusCode::SUCCESS;
}

std::vector<std::string> TrigBtagEmulationTool::addEmulatedChain(const std::string triggerMenu) {

  std::map< std::string,std::string > configuration;

  // Load the full menus for a specific year
  if ( triggerMenu.find("2015")!=std::string::npos )
    configuration.insert( std::get< TriggerMenu::YEAR_2015 >( m_triggerMenus ).begin(),
			  std::get< TriggerMenu::YEAR_2015 >( m_triggerMenus ).end() );
  if ( triggerMenu.find("2016")!=std::string::npos )
    configuration.insert( std::get< TriggerMenu::YEAR_2016 >( m_triggerMenus ).begin(),
			  std::get< TriggerMenu::YEAR_2016 >( m_triggerMenus ).end() );
  if ( triggerMenu.find("2017")!=std::string::npos )
    configuration.insert( std::get< TriggerMenu::YEAR_2017 >( m_triggerMenus ).begin(),
			  std::get< TriggerMenu::YEAR_2017 >( m_triggerMenus ).end() );
  if ( triggerMenu.find("2018")!=std::string::npos )
    configuration.insert( std::get< TriggerMenu::YEAR_2018 >( m_triggerMenus ).begin(),
			  std::get< TriggerMenu::YEAR_2018 >( m_triggerMenus ).end() );

  // Load specific chains in the menus
  if ( configuration.size() == 0 ) {
    if ( std::get< TriggerMenu::YEAR_2015 >( m_triggerMenus ).find(triggerMenu) != std::get< TriggerMenu::YEAR_2015 >( m_triggerMenus ).end() )
      configuration[ triggerMenu ] = std::get< TriggerMenu::YEAR_2015 >( m_triggerMenus ).at( triggerMenu );
    else if ( std::get< TriggerMenu::YEAR_2016 >( m_triggerMenus ).find(triggerMenu) != std::get< TriggerMenu::YEAR_2016 >( m_triggerMenus ).end() )
      configuration[ triggerMenu ] = std::get< TriggerMenu::YEAR_2016 >( m_triggerMenus ).at( triggerMenu );
    else if ( std::get< TriggerMenu::YEAR_2017 >( m_triggerMenus ).find(triggerMenu) != std::get< TriggerMenu::YEAR_2017 >( m_triggerMenus ).end() )
      configuration[ triggerMenu ] = std::get< TriggerMenu::YEAR_2017 >( m_triggerMenus ).at( triggerMenu );
    else if ( std::get< TriggerMenu::YEAR_2018 >( m_triggerMenus ).find(triggerMenu) != std::get< TriggerMenu::YEAR_2018 >( m_triggerMenus ).end() )
      configuration[ triggerMenu ] = std::get< TriggerMenu::YEAR_2018 >( m_triggerMenus ).at( triggerMenu );
  }
  
  std::vector<std::string> output;
  for (std::map< std::string,std::string >::const_iterator it=configuration.begin(); it!=configuration.end(); it++) {
    std::vector< std::string > trigger;
    trigger.push_back( it->first );
    output.push_back( it->first );
    
    std::istringstream sname( it->second );
    std::string token;
    while(std::getline( sname , token, ','))
      trigger.push_back( Form("EMUL_%s",token.c_str()) );
    
    this->addEmulatedChain( trigger );
  }
  return output;
}

//!==========================================================================
bool TrigBtagEmulationTool::isPassed(const std::string &chain) {
  // CHECK IF THE TOOK HAS ALREADY BEEN EXECUTED FOR THIS EVENT
  const xAOD::EventInfo* eventInfo = 0;
  ANA_CHECK( evtStore()->retrieve( eventInfo,"EventInfo" )); 

  bool isMC = false;
  if( eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) )
    isMC = true;

  long long int eventID = 0;
  if (isMC) eventID = eventInfo->mcEventNumber();
  else eventID = eventInfo->eventNumber();

  if ( eventID != m_previousEvent)
    ANA_CHECK( this->execute() );

  m_previousEvent = eventID;

  // CHECK IF CHAIN IS DEFINED AND RETURN RESULT
  auto result = m_emulatedChains.find(chain);
  if (result == m_emulatedChains.end()) return false;
  else return result->second.isPassed();
}

//!==========================================================================
void TrigBtagEmulationTool::clear() {
  ATH_MSG_DEBUG( "Clearing tool " << name() );

  // CLEANUP
  for (auto & emulatedChain : m_emulatedChains)
    emulatedChain.second.clear();
}

bool TrigBtagEmulationTool::hasSplit() {return m_splitTrigger;}
bool TrigBtagEmulationTool::hasBtag() {return m_btagTrigger;}
bool TrigBtagEmulationTool::hasHT() {return m_htTrigger;}
bool TrigBtagEmulationTool::hasGSC() {return m_gscTrigger;}

// *******

const xAOD::JetContainer* TrigBtagEmulationTool::retaggedJets(const std::string &collection) const {

  xAOD::JetContainer* output_jets = new xAOD::JetContainer(SG::OWN_ELEMENTS);
  xAOD::JetAuxContainer* output_jetsAux = new xAOD::JetAuxContainer;
  output_jets->setStore(output_jetsAux);

  xAOD::BTaggingContainer* output_btags = new xAOD::BTaggingContainer();
  xAOD::BTaggingAuxContainer*  output_btagsAux = new xAOD::BTaggingAuxContainer();
  output_btags->setStore(output_btagsAux);

  //  std::vector< TrigBtagEmulationJet* > jets;
  std::vector< std::unique_ptr< TrigBtagEmulationJet > > jets;

  if (collection == "EF") jets = m_manager_ef->getJets();
  else if (collection == "SPLIT") jets = m_manager_split->getJets();
  else if (collection == "GSC") jets = m_manager_gsc->getJets();
  else if (collection == "SPLIT_GSC") jets = m_manager_split_gsc->getJets();
  else {
    ATH_MSG_ERROR("Trying to retrieve non-existing re-tagged jet collection '"<< collection << "'");
    ATH_MSG_ERROR("Available re-tagged jet collections are : 'EF', 'SPLIT', 'GSC', 'SPLIT_GSC'");
    ATH_MSG_FATAL("Will return empty JetContainer");
    return output_jets;
  }

  for (unsigned int i(0); i<jets.size(); i++) {

    bool isDefault = true;
    std::map< std::string , double > weights = jets.at(i)->weights();

    std::map< std::string, double >::const_iterator it = weights.begin();
    for ( ; it!=weights.end(); it++ ) {
      std::string taggerName = it->first;
      if ( taggerName.find("MV2")!=std::string::npos && it->second != -1) isDefault = false;
      if ( taggerName.find("MV2")==std::string::npos && it->second != -1000 ) isDefault = false;
    }
    if ( isDefault ) continue;

    xAOD::Jet *theJet = jets.at(i)->jet();
    xAOD::BTagging *theBtag = new xAOD::BTagging();

    output_jets->push_back( theJet );
    output_btags->push_back( theBtag );

    for ( it=weights.begin() ; it!=weights.end(); it++ ) 
      theBtag->setVariable< double >( it->first.c_str(), "discriminant", it->second );

    ElementLink< xAOD::BTaggingContainer> linkBTagger;
    linkBTagger.toContainedElement(*output_btags, theBtag);
    theJet->setBTaggingLink(linkBTagger);
  }

  return output_jets;
}

std::vector< std::vector< std::string > > TrigBtagEmulationTool::definition(const std::string& chainName) const {
  if ( m_emulatedChains.find( chainName ) != m_emulatedChains.end() )
    return m_emulatedChains.at( chainName ).definition();
  ATH_MSG_WARNING( "Trying to retrieve trigger definition for a non-existent chain : '" << chainName << "'" );
  ATH_MSG_WARNING( "Returning empty definition" );
  return std::vector< std::vector< std::string > >();
}

// ******

bool TrigBtagEmulationTool::checkTriggerChain(const std::vector<std::string>& toBeEmulatedChain) {
  std::vector< std::string >::const_iterator it = toBeEmulatedChain.begin();
  if ( toBeEmulatedChain.size() > 1 ) it++;

  for ( ; it!=toBeEmulatedChain.end(); it++) {
    std::string triggerName = *it;
    if ( triggerName.find("split")!=std::string::npos ) m_splitTrigger = true; 
    if ( triggerName.find("_b")!=std::string::npos )    m_btagTrigger  = true;
    if ( triggerName.find("ht")!=std::string::npos )    m_htTrigger    = true;  
    if ( triggerName.find("gsc")!=std::string::npos )   m_gscTrigger   = true; 
  }

  return true;
}

StatusCode TrigBtagEmulationTool::retrieve( std::unique_ptr< Trig::JetManager >& manager, bool useTriggerNavigation) {

  StatusCode sc = StatusCode::FAILURE;
  if (useTriggerNavigation) sc = manager->retrieveByNavigation();
  else sc = manager->retrieveByContainer();

  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Could not retrieve Jets!" );
    return sc;
  }

  const std::string chainName = manager->chainName();
  const std::string jetContainerName = manager->jetContainerName();
  const std::string btaggingContainerName = manager->btaggingContainerName();

  const unsigned int jetSize= manager->jetSize();
  const unsigned int jetRoISize = manager->jetRoISize();
  const unsigned int btaggingSize= manager->btaggingSize();
  const unsigned int primaryVertexSize = manager->primaryVertexSize();
  const unsigned int trackParticleSize = manager->trackParticleSize();

  if (!useTriggerNavigation) {
    ATH_MSG_DEBUG( "Size of input containers ['" << jetContainerName <<"' , '" << 
		   btaggingContainerName << "'] : " <<
		   "jet=" << jetSize << " " <<
		   "jetRoI=" << jetRoISize << " " <<
		   "btag=" << btaggingSize << " " <<
		   "vtx=" << primaryVertexSize << " " <<
		   "trk=" << trackParticleSize << " " );
  } else {
    ATH_MSG_DEBUG( "Size of input containers ['" << chainName << "'] : " <<
		   "jet=" << jetSize << " " <<
		   "jetRoI=" << jetRoISize << " " <<
		   "btag=" << btaggingSize << " " <<
		   "vtx=" << primaryVertexSize << " " <<
		   "trk=" << trackParticleSize << " " );
  }

  return sc;
}


template<typename T> StatusCode TrigBtagEmulationTool::retrieveTool(T& myTool) {
  if (myTool.retrieve().isFailure()) {
    ATH_MSG_FATAL( "Failed to locate tool " << myTool );
    return StatusCode::FAILURE;
  } 

  ATH_MSG_DEBUG( "Retrieved tool " << myTool );
  return StatusCode::SUCCESS;
}


StatusCode TrigBtagEmulationTool::checkInputChainExists( const std::string &inputChain ) const {
  const Trig::ChainGroup *chainGroup = m_trigDec->getChainGroup( inputChain );

  if ( chainGroup->getListOfTriggers().size() == 0 ) {
    ATH_MSG_ERROR( "Input Chain ('" << inputChain << "') is not in the list of available triggers" );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
