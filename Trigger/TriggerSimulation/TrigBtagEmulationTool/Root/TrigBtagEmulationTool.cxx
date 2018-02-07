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

  declareProperty("InputChain",                  m_input_chain="HLT_j35_boffperf");
  declareProperty("InputChainSplit",             m_input_chainSplit="HLT_j35_boffperf_split");
  declareProperty("InputChainGSC",               m_input_chainGSC="HLT_j15_gsc35_boffperf_split");

  // EF keys
  declareProperty("InputPrimaryVertexKey",       m_input_pvKey="EFHistoPrmVtx");
  declareProperty("InputJetKey",                 m_input_jetKey="");
  declareProperty("InputTrackParticleKey",       m_input_tpKey="");

  // SPLIT keys
  declareProperty("InputPrimaryVertexKey_Split", m_input_pvKeySplit="xPrimVx");
  declareProperty("InputJetKey_Split",           m_input_jetKeySplit="SplitJet");
  declareProperty("InputTrackParticleKey_Split", m_input_tpKeySplit="InDetTrigTrackingxAODCnv_Bjet_IDTrig");

  // GSC keys
  declareProperty("InputPrimaryVertexKey_GSC",   m_input_pvKey_GSC ="xPrimVx"); 
  declareProperty("InputJetKey_GSC",             m_input_jetKey_GSC="GSCJet");
  declareProperty("InputTrackParticleKey_GSC",   m_input_tpKey_GSC ="InDetTrigTrackingxAODCnv_Bjet_IDTrig"); 

  declareProperty("InputBTaggingName",           m_input_btagName="HLT_xAOD__BTaggingContainer_HLTBjetFex");

  declareProperty("InputJetName",                m_input_jetName="HLT_xAOD__JetContainer_EFJet");
  declareProperty("InputJetName_Split",          m_input_jetNameSplit="HLT_xAOD__JetContainer_SplitJet");

  declareProperty("TriggerMenu"             ,    m_trigger_menu="v7");

  declareProperty("InputJetName_GSC",            m_input_JetName_GSC="HLT_xAOD__JetContainer_GSCJet" );

  declareProperty("EmulatedChainDefinitions",    m_emulatedChainDefinitions);
  declareProperty("AutoConfigMenu"          ,    m_autoconfiguredMenu="");

  declareProperty("Verbosity",            m_verbosity=0);

#if defined( XAOD_STANDALONE )
  declareProperty("TrigDecisionToolName", m_TrigDecToolName = "Trig::TrigDecisionTool/TrigDecisionTool");
  declareProperty("xAODConfigToolName"  , m_xAODConfToolName = "TrigConf::xAODConfigTool");
#endif
}

//**********************************************************************

StatusCode TrigBtagEmulationTool::initialize() {
  
  ATH_MSG_INFO( "Initialising tool " << name() );

  // RETRIEVE SERVICES
  // Retrieve StoreGate service
  StatusCode sc;

#if !defined( XAOD_STANDALONE )
  sc = m_storeGate.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Could not retrieve StoreGateSvc!" );
    return sc;
  }

  // Retrieve TrigDecisionTool
  sc = m_trigDec.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Could not retrieve TrigDecisionTool!" );
    return sc;
  }
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

  // Retrieve the offline track association tool
  if (!m_bTagTrackAssocTool.empty()) {
    if (m_bTagTrackAssocTool.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Failed to locate tool " << m_bTagTrackAssocTool );
      return StatusCode::FAILURE;
    } else
      ATH_MSG_INFO( "Retrieved tool " << m_bTagTrackAssocTool );
  } else
    ATH_MSG_DEBUG( "No track association tool to retrieve" );

  // Retrieve the bTagSecVtxTool
  if (m_bTagSecVtxTool.retrieve().isFailure()) {
    ATH_MSG_FATAL( "Failed to locate tool " << m_bTagSecVtxTool );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO( "Retrieved tool " << m_bTagSecVtxTool );

  // Retrieve the main BTagTool
  if (m_bTagTool.retrieve().isFailure()) {
    ATH_MSG_FATAL( "Failed to locate tool " << m_bTagTool );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO( "Retrieved tool " << m_bTagTool );

#endif


  // Autoconfiguration of Trigger Menu
  if (this->initTriggerChainsMenu().isFailure() ) {
    ATH_MSG_ERROR( "Could not initialize lowest unprescaled Trigger Chains!" );
    return StatusCode::FAILURE;
  }
  if (m_autoconfiguredMenu == "2015Menu" || m_autoconfiguredMenu == "2016Menu" || m_autoconfiguredMenu == "2017Menu" ||
      m_autoconfiguredMenu == "2015+2016Menu" ||  m_autoconfiguredMenu == "2015+2017Menu" || m_autoconfiguredMenu == "2016+2017Menu" ||
      m_autoconfiguredMenu == "2015+2016+2017Menu" ) {
    ATH_MSG_INFO( Form( "Automatic configuration of Trigger Chains for %s",m_autoconfiguredMenu.c_str()) );
    ATH_MSG_INFO( Form( "For full list of trigger chains automatically loaded look here : '%s'","https://twiki.cern.ch/twiki/bin/view/Atlas/TrigBjetEmulation") );
    this->addEmulatedChain( m_autoconfiguredMenu );
  }
  
  // CREATE EMULATED CHAINS
  for (auto definition : m_emulatedChainDefinitions) {
    checkTriggerChain(definition);

    std::vector< std::string > chainDefinition( definition.begin(), definition.end() );
    // Check if trigger is already stored as a lower-unprescaled trigger
    if (chainDefinition.size() == 1) {
      std::string triggerSubComponents = "";
      if ( m_2015Menu.find( chainDefinition.at(0) ) != m_2015Menu.end() )
	triggerSubComponents = m_2015Menu.at( chainDefinition.at(0) );
      else if ( m_2016Menu.find( chainDefinition.at(0) ) != m_2016Menu.end() )
	triggerSubComponents = m_2016Menu.at( chainDefinition.at(0) );
      else if ( m_2017Menu.find( chainDefinition.at(0) ) != m_2017Menu.end() )
	triggerSubComponents = m_2017Menu.at( chainDefinition.at(0) );

      std::istringstream sname( triggerSubComponents );
      std::string token;
      while(std::getline( sname , token, ','))
	chainDefinition.push_back( Form("EMUL_%s",token.c_str()) );
    }

    TrigBtagEmulationChain chain(TrigBtagEmulationChain(chainDefinition,m_trigDec));

    if ( chain.isAutoConfigured() ) {
      ATH_MSG_INFO( "AUTOMATIC PARSER has been activated for trigger chain : " << chain.getName() );
      std::vector< std::string > subChains = chain.retrieveAutoConfiguration();
      ATH_MSG_INFO("This trigger has been configured as the logical AND of the folowing triggers : ");
      for (unsigned int i=0; i<subChains.size(); i++) 
	ATH_MSG_INFO( "  -- " << subChains.at(i) );
    }
    if ( !chain.isCorrectlyConfigured() ) {
      ATH_MSG_FATAL("NOT correct Configuration for Trigger Chain : " << chain.getName() << ".");
      return StatusCode::FAILURE;
    }

    m_emulatedChains.insert(std::make_pair(chain.getName(),chain));
  }

  // jet Menagers
  m_manager_ef = std::unique_ptr< Trig::JetManager >( new Trig::JetManager(m_trigDec,m_input_chain,m_input_jetName,m_input_btagName) );
  m_manager_split = std::unique_ptr< Trig::JetManager >( new Trig::JetManager(m_trigDec,m_input_chainSplit,m_input_jetNameSplit,m_input_btagName) );
  m_manager_gsc = std::unique_ptr< Trig::JetManager >( new  Trig::JetManager(m_trigDec,m_input_chainGSC,m_input_JetName_GSC,m_input_btagName) );
  m_manager_ef_gsc = std::unique_ptr< Trig::JetManager >( new Trig::JetManager(m_trigDec,"HLT_j15_boffperf",m_input_JetName_GSC,m_input_btagName) );
  m_manager_split_gsc = std::unique_ptr< Trig::JetManager >( new Trig::JetManager(m_trigDec,m_input_chainGSC,m_input_JetName_GSC,m_input_btagName) );

  m_manager_ef->setKeys(m_input_jetKey,m_input_pvKey,m_input_tpKey);
  m_manager_split->setKeys(m_input_jetKeySplit,m_input_pvKeySplit,m_input_tpKeySplit);
  m_manager_gsc->setKeys(m_input_jetKey_GSC,m_input_pvKey_GSC,m_input_tpKey_GSC);
  m_manager_ef_gsc->setKeys(m_input_jetKey,m_input_pvKey,m_input_tpKey);
  m_manager_split_gsc->setKeys(m_input_jetKeySplit,m_input_pvKeySplit,m_input_tpKeySplit);


#if !defined( XAOD_STANDALONE ) && !defined( XAOD_ANALYSIS )
  JetManager::m_bTagTool = &m_bTagTool;
  JetManager::m_bTagTrackAssocTool = &m_bTagTrackAssocTool;
  JetManager::m_bTagSecVtxTool = &m_bTagSecVtxTool;
#endif

  return sc;
}


//!==========================================================================
StatusCode TrigBtagEmulationTool::execute() {

  if (m_verbosity > 0) ATH_MSG_INFO( "Executing tool " << name() );

  // CLEAR PREVIOUS RESULTS
  clear();

  // RETRIEVE L1 JETS
  const xAOD::JetRoIContainer *l1JetContainer = 0;
#ifndef XAOD_STANDALONE
  CHECK( evtStore()->retrieve(l1JetContainer,"LVL1JetRoIs") );
#else
  ANA_CHECK( evtStore()->retrieve(l1JetContainer,"LVL1JetRoIs") );
#endif

  std::vector< TrigBtagEmulationJet* > l1_jets;
  std::vector< TrigBtagEmulationJet* > l1_jets_jj;

  for (auto & l1jet : *l1JetContainer) {
    TrigBtagEmulationJet *newJet = new TrigBtagEmulationJet( l1jet );
    TrigBtagEmulationJet *newJetJJ = new TrigBtagEmulationJet( l1jet,true ); 

    bool unique=true;
    for (auto & l1_jet : l1_jets) {
      if (newJet->pt()  == l1_jet->pt() &&
	  newJet->eta() == l1_jet->eta() &&
	  newJet->phi() == l1_jet->phi()) { unique = false; break; }
    }

    if(unique) { l1_jets.push_back( newJet ); l1_jets_jj.push_back( newJetJJ ); }
    else { delete newJet; delete newJetJJ; }
  }


  // RETRIEVE INPUT CONTAINER VECTORS  // TODO - CHECK JET==BTAG SIZE
  // Retrieve raw jets for HT computation
  std::string HTContainer = m_trigger_menu.find("v7")!=std::string::npos ? "HLT_xAOD__JetContainer_a4tcemsubjesISFS" : "HLT_xAOD__JetContainer_a4tcemsubjesFS";
  std::vector<const xAOD::Jet*> input_jetsHT;

  if (getInputContainerSG(input_jetsHT,m_input_chainSplit,HTContainer).isFailure()) {
    ATH_MSG_ERROR( "Could not retrieve Input Containers!" );
    return StatusCode::FAILURE;
  }
  if (m_verbosity > 0) ATH_MSG_INFO ("Size of input HT containers (" << HTContainer << "):" << " jet=" << input_jetsHT.size());

  // Retrieve Jets
#ifndef XAOD_STANDALONE
  CHECK( retrieve( m_manager_ef   ,m_useTriggerNavigation ) );
  CHECK( retrieve( m_manager_split,m_useTriggerNavigation ) );
  if ( this->hasGSC() ) CHECK( retrieve( m_manager_gsc       ,m_useTriggerNavigation ) );
  if ( this->hasGSC() ) CHECK( retrieve( m_manager_ef_gsc    ,m_useTriggerNavigation ) );
  if ( this->hasGSC() ) CHECK( retrieve( m_manager_split_gsc ,m_useTriggerNavigation ) );
#else
  ANA_CHECK( retrieve( m_manager_ef   ,m_useTriggerNavigation ) );
  ANA_CHECK( retrieve( m_manager_split,m_useTriggerNavigation ) );
  if ( this->hasGSC() ) ANA_CHECK( retrieve( m_manager_gsc       ,m_useTriggerNavigation ) );
  if ( this->hasGSC() ) ANA_CHECK( retrieve( m_manager_ef_gsc    ,m_useTriggerNavigation ) );
  if ( this->hasGSC() ) ANA_CHECK( retrieve( m_manager_split_gsc ,m_useTriggerNavigation ) );
#endif
  
  
  // RETAG COPYING ORIGINAL WEIGHTS 
  if (!m_useTriggerNavigation || !m_tagOfflineWeights || !m_tagOnlineWeights) {
#ifndef XAOD_STANDALONE
    CHECK( m_manager_ef->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
    CHECK( m_manager_split->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
    CHECK( m_manager_gsc->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
    CHECK( m_manager_ef_gsc->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
    CHECK( m_manager_split_gsc->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
#else
    ANA_CHECK( m_manager_ef->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
    ANA_CHECK( m_manager_split->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
    ANA_CHECK( m_manager_gsc->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
    ANA_CHECK( m_manager_ef_gsc->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
    ANA_CHECK( m_manager_split_gsc->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
#endif      
  }
  // RETAG WITH OFFLINE TOOLS    
  if (m_useTriggerNavigation && m_tagOfflineWeights) {
#ifndef XAOD_STANDALONE
    CHECK( m_manager_ef->retagOffline()        );
    CHECK( m_manager_split->retagOffline()     );
    CHECK( m_manager_gsc->retagOffline()       );
    CHECK( m_manager_ef_gsc->retagOffline()    );
    CHECK( m_manager_split_gsc->retagOffline() );
#else
    ANA_CHECK( m_manager_ef->retagOffline()        );
    ANA_CHECK( m_manager_split->retagOffline()     );
    ANA_CHECK( m_manager_gsc->retagOffline()       );
    ANA_CHECK( m_manager_ef_gsc->retagOffline()    );
    ANA_CHECK( m_manager_split_gsc->retagOffline() );
#endif
  }
  // RETAG WITH ONLINE TOOLS
  if (m_useTriggerNavigation && m_tagOnlineWeights) {
#ifndef XAOD_STANDALONE
    CHECK( m_manager_ef->retagOnline()        );
    CHECK( m_manager_split->retagOnline()     );
    CHECK( m_manager_gsc->retagOnline()       );
    CHECK( m_manager_ef_gsc->retagOnline()    );
    CHECK( m_manager_split_gsc->retagOnline() );
#else
    ANA_CHECK( m_manager_ef->retagOnline()        );
    ANA_CHECK( m_manager_split->retagOnline()     );
    ANA_CHECK( m_manager_gsc->retagOnline()       );
    ANA_CHECK( m_manager_ef_gsc->retagOnline()    );
    ANA_CHECK( m_manager_split_gsc->retagOnline() );
#endif
  }
  
  // BACKUP NON-CENTRAL JET INFO FOR SPLIT CHAINS  
  *m_manager_ef += *m_manager_split;
  *m_manager_split += *m_manager_ef;
  m_manager_ef->merge(input_jetsHT); 
  m_manager_split->merge(input_jetsHT); 

  *m_manager_ef_gsc += *m_manager_split_gsc;
  *m_manager_split_gsc += *m_manager_ef_gsc;
  m_manager_ef_gsc->merge(input_jetsHT); 
  m_manager_split_gsc->merge(input_jetsHT); 

  // EVALUATE EMULATED CHAINS
  for (auto & emulatedChain : m_emulatedChains) {
    emulatedChain.second.addJet("LVL1",l1_jets);
    emulatedChain.second.addJet("LVL1_JJ",l1_jets_jj);
    emulatedChain.second.addJet("EF",m_manager_ef->getJets());
    emulatedChain.second.addJet("SPLIT",m_manager_split->getJets());
    emulatedChain.second.addJet("GSC",m_manager_gsc->getJets());
    emulatedChain.second.addJet("EF_GSC",m_manager_ef_gsc->getJets());
    emulatedChain.second.addJet("SPLIT_GSC",m_manager_split_gsc->getJets());

    emulatedChain.second.evaluate();
  }

  if (m_verbosity > 0) {
    // DUMP L1 JETS
    ATH_MSG_INFO ("L1 jets");
    for (unsigned int i=0; i<l1_jets.size(); i++) {
      auto & l1_jet    = l1_jets.at(i);
      auto & l1_jet_jj = l1_jets_jj.at(i);
      ATH_MSG_INFO ("  Jet --- pt[et8x8]=" << l1_jet->pt() << " pt[et4x4]="<< l1_jet_jj->pt()  << " eta=" << l1_jet->eta() << " phi=" << l1_jet->phi() );
    }
    
    // DUMP EF JETS
    ATH_MSG_INFO ("EF jets");
    for (auto & jet : m_manager_ef->getJets()) {
      ATH_MSG_INFO ("  Jet --- pt=" << jet->pt()*1e-3 << " eta=" << jet->eta() << " phi=" << jet->phi());
      for (auto & weight : jet->weights()) 
	ATH_MSG_INFO ("      " << weight.first << " " << weight.second);
    }
    
    // DUMP SPLIT JETS
    ATH_MSG_INFO ("SPLIT jets");
    for (auto & jet : m_manager_split->getJets()) {
      ATH_MSG_INFO ("  Jet --- pt=" << jet->pt()*1e-3 << " eta=" << jet->eta() << " phi=" << jet->phi());
      for (auto & weight : jet->weights())
	ATH_MSG_INFO ("      " << weight.first << " " << weight.second);
    }
    
    // DUMP GSC JETS
    ATH_MSG_INFO ("GSC jets");
    for (auto & jet : m_manager_gsc->getJets()) {
      ATH_MSG_INFO ("  Jet --- pt=" << jet->pt()*1e-3 << " eta=" << jet->eta());
      for (auto & weight : jet->weights())
	ATH_MSG_INFO ("      " << weight.first << " " << weight.second);
    }

    // DUMP EMULATED DECISIONS
    ATH_MSG_INFO ("Emulated decisions");
    for (auto & emulatedChain : m_emulatedChains) {
      if (m_verbosity > 1) emulatedChain.second.Print();
      ATH_MSG_INFO ("  Chain --- name=" << emulatedChain.first << " result=" << (int)emulatedChain.second.isPassed());
    }
    
  }
  
  for (unsigned int i(0); i<l1_jets.size(); i++) delete l1_jets.at(i);
  for (unsigned int i(0); i<l1_jets_jj.size(); i++) delete l1_jets_jj.at(i);

  return StatusCode::SUCCESS;
}


//!==========================================================================
StatusCode TrigBtagEmulationTool::finalize() {
  ATH_MSG_INFO( "Finalising tool " << name() );
  return StatusCode::SUCCESS;
}


//!==========================================================================
StatusCode TrigBtagEmulationTool::addEmulatedChain(const std::vector<std::string>& chainDescription) {
  // check if gsc chains are present in the emulation
  checkTriggerChain(chainDescription);

  // Create chain and add it to the list
  TrigBtagEmulationChain chain(TrigBtagEmulationChain(chainDescription,m_trigDec));
  // Check it has been auto configured

  if ( chain.isAutoConfigured() ) {
    ATH_MSG_INFO( "AUTOMATIC PARSER has been activated for trigger chain : " << chain.getName() );
    std::vector< std::string > subChains = chain.retrieveAutoConfiguration();
    ATH_MSG_INFO("This trigger has been configured as the logical AND of the folowing triggers : ");
    for (unsigned int i=0; i<subChains.size(); i++)
      ATH_MSG_INFO( "  -- " << subChains.at(i) );
  }
  // Check it has been conrrectyle configured
  if ( !chain.isCorrectlyConfigured() ) {
    ATH_MSG_FATAL("NOT correct Configuration for Trigger Chain : " << chain.getName() << "." );
    return StatusCode::FAILURE;
  }

  m_emulatedChains.insert(std::make_pair(chain.getName(),chain));
  return StatusCode::SUCCESS;
}

//!==========================================================================        

StatusCode TrigBtagEmulationTool::initTriggerChainsMenu() {
  const std::string nameFile_2015 = PathResolverFindDataFile( "TrigBtagEmulationTool/triggerChains_2015Menu.txt" );
  const std::string nameFile_2016 = PathResolverFindDataFile( "TrigBtagEmulationTool/triggerChains_2016Menu.txt" );
  const std::string nameFile_2017 = PathResolverFindDataFile( "TrigBtagEmulationTool/triggerChains_2017Menu.txt" );

  // *** 2015 Chains
  std::ifstream file2015; file2015.open( nameFile_2015.c_str() );
  if (!file2015) {
    ATH_MSG_ERROR( "Could not load lowest unprescaled Trigger Chains for 2015 !" );
    return StatusCode::FAILURE;
  }
  while ( !file2015.eof() ) {
    std::string line;
    getline (file2015,line);
    if (line.empty()) continue;
    std::istringstream is_line(line);
    std::string chainName, chainComponents;
    is_line >> chainName >> chainComponents;
    m_2015Menu[chainName] = chainComponents; 
  }
  file2015.close();

  // *** 2016 Chains
  std::ifstream file2016; file2016.open( nameFile_2016.c_str() );
  if (!file2016) {
    ATH_MSG_ERROR( "Could not load lowest unprescaled Trigger Chains for 2016 !" );
    return StatusCode::FAILURE;
  }
  while ( !file2016.eof() ) {
    std::string line;
    getline (file2016,line);
    if (line.empty()) continue;
    std::istringstream is_line(line);
    std::string chainName, chainComponents;
    is_line >> chainName >> chainComponents;
    m_2016Menu[chainName] = chainComponents; 
  }
  file2016.close();

  // *** 2017 Chains
  std::ifstream file2017; file2017.open( nameFile_2017.c_str() );
  if (!file2017) {
    ATH_MSG_ERROR( "Could not load lowest unprescaled Trigger Chains for 2017 !" );
    return StatusCode::FAILURE;
  }
  while ( !file2017.eof() ) {
    std::string line;
    getline (file2017,line);
    if (line.empty()) continue;
    std::istringstream is_line(line);
    std::string chainName, chainComponents;
    is_line >> chainName >> chainComponents;
    m_2017Menu[chainName] = chainComponents;
  }
  file2017.close();

  return StatusCode::SUCCESS;
}

std::vector<std::string> TrigBtagEmulationTool::addEmulatedChain(const std::string triggerMenu) {

  std::map< std::string,std::string > configuration;
  if ( triggerMenu.find("2015")!=std::string::npos )
    configuration.insert( m_2015Menu.begin(),m_2015Menu.end() );
  if ( triggerMenu.find("2016")!=std::string::npos )
    configuration.insert( m_2016Menu.begin(),m_2016Menu.end() );
  if ( triggerMenu.find("2017")!=std::string::npos )
    configuration.insert( m_2017Menu.begin(),m_2017Menu.end() );

  if ( configuration.size() == 0 ) {
    if ( m_2015Menu.find(triggerMenu) != m_2015Menu.end() )
      configuration[ triggerMenu ] = m_2015Menu.at(triggerMenu);
    else if ( m_2016Menu.find(triggerMenu) != m_2016Menu.end() )
      configuration[ triggerMenu ] = m_2016Menu.at(triggerMenu);
    else if ( m_2017Menu.find(triggerMenu) != m_2017Menu.end() )
      configuration[ triggerMenu ] = m_2017Menu.at(triggerMenu);
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
#ifndef XAOD_STANDALONE
  CHECK( evtStore()->retrieve( eventInfo,"EventInfo" ));
#else
  ANA_CHECK( evtStore()->retrieve( eventInfo,"EventInfo" )); 
#endif

  bool isMC = false;
  if( eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) )
    isMC = true;

  long long int eventID = 0;
  if (isMC) eventID = eventInfo->mcEventNumber();
  else eventID = eventInfo->eventNumber();

  if ( eventID != m_previousEvent)
#ifndef XAOD_STANDALONE
    CHECK( this->execute() );
#else
    ANA_CHECK( this->execute() );
#endif
  m_previousEvent = eventID;

  // CHECK IF CHAIN IS DEFINED AND RETURN RESULT
  auto result = m_emulatedChains.find(chain);
  if (result == m_emulatedChains.end()) return false;
  else return result->second.isPassed();
}

//!==========================================================================
void TrigBtagEmulationTool::clear() {

  if (m_verbosity > 0) ATH_MSG_INFO( "Clearing tool " << name() );

  // CLEANUP
  for (auto & emulatedChain : m_emulatedChains) {
    emulatedChain.second.clear();
  }
  
}

StatusCode TrigBtagEmulationTool::getInputContainerSG(std::vector<const xAOD::Jet*>& jetContainers,std::string& inputItem, std::string& jetName) {

  // Check if the input chain fired
  if (!m_trigDec->isPassed(inputItem)) return StatusCode::SUCCESS;

  // Get Jet objects
  const xAOD::JetContainer *sgJetContainer = 0;

#ifndef XAOD_STANDALONE
  CHECK( evtStore()->retrieve(sgJetContainer,jetName) );
#else
  ANA_CHECK( evtStore()->retrieve(sgJetContainer,jetName) );
#endif

  if (sgJetContainer == 0) {
    ATH_MSG_ERROR( "Retrieved invalid Jet Input Container!" );
    return StatusCode::FAILURE;
  }

  bool isJetUnique=true;
  for (const auto &sgj : *sgJetContainer) {
    for (const auto &j : jetContainers)
      if(sgj==j) isJetUnique=false;
    if (isJetUnique) jetContainers.push_back(sgj);
  }
  
  return StatusCode::SUCCESS;
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

  std::vector< TrigBtagEmulationJet* > jets;

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
  StatusCode sc = StatusCode::SUCCESS;

  if (useTriggerNavigation) sc = manager->retrieveByNavigation();
  else sc = manager->retrieveByContainer( evtStore() );

  if ( sc.isFailure() ) 
    ATH_MSG_ERROR( "Could not retrieve Jets!" );
  else if (m_verbosity > 0 && useTriggerNavigation)
    ATH_MSG_INFO( Form( "Size of input containers (%s): jet=%lu vtx==%lu trk=%lu btg=%lu",manager->m_chain.c_str(),
			manager->m_jet_Containers.size(), manager->m_primaryVertex_Containers.size(),
			manager->m_trackParticle_Containers.size(),manager->m_btagging_Containers.size()) );
  else if (m_verbosity > 0)
    ATH_MSG_INFO( Form( "Size of input containers (%s) [%s,%s]: jet=%lu vtx==%lu trk=%lu btg=%lu",
			manager->m_chain.c_str(), manager->m_jetContainer.c_str(), manager->m_btagContainer.c_str(),
			manager->m_jet_Containers.size(), manager->m_primaryVertex_Containers.size(),
			manager->m_trackParticle_Containers.size(), manager->m_btagging_Containers.size()) );

  return sc;
}


