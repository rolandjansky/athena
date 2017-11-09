/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration 
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

    std::vector< baseTrigBtagEmulationChainJetIngredient* > triggerSubComponents;
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
void TrigBtagEmulationChain::addDecisionIngredient(baseTrigBtagEmulationChainJetIngredient* decision) { m_ingredientsJet.push_back( decision ); }

bool TrigBtagEmulationChain::parseChainName( std::string triggerName, std::vector< baseTrigBtagEmulationChainJetIngredient* >& trigger_subComponents) {
  std::vector< std::string > parsedTriggers;

  if ( triggerName.find("_AND_")!=std::string::npos ) {
    std::string trigger_AND_trigA = triggerName.substr( 0,triggerName.find("_AND_") );
    std::string trigger_AND_trigB = "HLT_" + triggerName.substr( triggerName.find("_AND_") + 5, triggerName.length() - triggerName.find("_AND_") - 5 );
    parsedTriggers.push_back( trigger_AND_trigA );
    parsedTriggers.push_back( trigger_AND_trigB );
  } else parsedTriggers.push_back( triggerName );

  for (unsigned int i(0); i<parsedTriggers.size(); i++) {
    triggerName = parsedTriggers.at(i);
    std::vector< baseTrigBtagEmulationChainJetIngredient* > tmp_trigger_subComponents;

    if (triggerName.find("HLT_")!=std::string::npos) tmp_trigger_subComponents = processHLTtrigger( triggerName );
    else if (triggerName.find("L1_")!=std::string::npos) tmp_trigger_subComponents = processL1trigger( triggerName );
    else m_correctlyConfigured = false;

    trigger_subComponents.insert( trigger_subComponents.end(), tmp_trigger_subComponents.begin(), tmp_trigger_subComponents.end());
  }

  return true;
}

std::vector< baseTrigBtagEmulationChainJetIngredient* > TrigBtagEmulationChain::processL1trigger (std::string input) {
  std::vector< baseTrigBtagEmulationChainJetIngredient* > output;

  while ( !input.empty() ) {
    std::string subString = "";

    bool hasSeparation = input.find("_") != std::string::npos;
    subString = hasSeparation ? input.substr( 0,input.find("_") ) : input;
    input = hasSeparation ? input.substr( input.find("_") + 1 , input.length() - input.find("_") -1 ) : "";

    if (subString == "L1") continue;
    else output.push_back( new TrigBtagEmulationChainJetIngredient_L1("EMUL_L1_" + subString) );
    output.at( output.size() - 1)->initialize();
  }

  return output;
}
std::vector< baseTrigBtagEmulationChainJetIngredient* > TrigBtagEmulationChain::processHLTtrigger (std::string input) {
  std::vector< baseTrigBtagEmulationChainJetIngredient* > output;

  std::string L1component  = "";
  std::string HLTcomponent = "";
  if (input.find("L1") != std::string::npos) {
    L1component  = input.substr( input.find("L1") , input.length() - input.find("L1") );
    HLTcomponent = input.substr( 0 , input.find("L1") - 1);
    L1component.replace( L1component.find("L1") + 2, 0 ,"_" );
  }

  // Deal with L1 Components
  if ( !L1component.empty() ) {
    std::vector< baseTrigBtagEmulationChainJetIngredient* > trigger_L1 = processL1trigger( L1component );
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
  for (unsigned int i(0); i<outputHLT_string.size(); i++)
    triggers_HLT.push_back( new TrigBtagEmulationChainJetIngredient_HLT( outputHLT_string.at(i) ) );
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

bool TrigBtagEmulationChain::addJet(std::string item,std::vector< struct TrigBtagEmulationJet >& jets) {
  for (unsigned int index(0); index<m_ingredientsJet.size(); index++)
    if (m_ingredientsJet.at(index)->needsJet(item)) m_ingredientsJet.at(index)->addJet(item,jets);
  return true;
}

//**********************************************************************

bool TrigBtagEmulationTool::checkTriggerChain(const std::vector<std::string>& toBeEmulatedChain) {

  for (unsigned int index(1); index < toBeEmulatedChain.size(); index++)
    {
      if (toBeEmulatedChain.at(index).find("split")!=std::string::npos) m_splitTrigger = true;
      if (toBeEmulatedChain.at(index).find("_b")!=std::string::npos)    m_btagTrigger  = true;
      if (toBeEmulatedChain.at(index).find("ht")!=std::string::npos)    m_htTrigger    = true;
      if (toBeEmulatedChain.at(index).find("gsc")!=std::string::npos)   m_gscTrigger   = true;
    }

  return true;
}

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
    m_manager_ef(nullptr),
    m_manager_split(nullptr),
    m_manager_gsc(nullptr),
    m_manager_ef_gsc(nullptr),
    m_manager_split_gsc(nullptr),
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
  if (this->initTriggerChainsMenu().isFailure() )
    {
      ATH_MSG_ERROR( "Could not initialize lowest unprescaled Trigger Chains!" );
      return StatusCode::FAILURE;
    }
  if (m_autoconfiguredMenu == "2015Menu" || m_autoconfiguredMenu == "2016Menu" || m_autoconfiguredMenu == "2017Menu" ||
      m_autoconfiguredMenu == "2015+2016Menu" ||  m_autoconfiguredMenu == "2015+2017Menu" || m_autoconfiguredMenu == "2016+2017Menu" ||
      m_autoconfiguredMenu == "2015+2016+2017Menu" )
    {
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

#if !defined( XAOD_STANDALONE ) && !defined( XAOD_ANALYSIS )
  jetManager::m_bTagTool = &m_bTagTool;
  jetManager::m_bTagTrackAssocTool = &m_bTagTrackAssocTool;
  jetManager::m_bTagSecVtxTool = &m_bTagSecVtxTool;
#endif

  return sc;
}


//!==========================================================================
StatusCode TrigBtagEmulationTool::execute() {

  if (m_verbosity > 0) ATH_MSG_INFO( "Executing tool " << name() );

  // CLEAR PREVIOUS RESULTS
  clear();

  // RETRIEVE L1 JETS
  std::vector<TrigBtagEmulationJet> l1_jets;
  const xAOD::JetRoIContainer *l1JetContainer = 0;

#ifndef XAOD_STANDALONE
  CHECK( evtStore()->retrieve(l1JetContainer,"LVL1JetRoIs") );
#else
  ANA_CHECK( evtStore()->retrieve(l1JetContainer,"LVL1JetRoIs") );
#endif

  for (auto & l1jet : *l1JetContainer) {
    TrigBtagEmulationJet newJet;
    newJet.pt  = l1jet->et8x8()*0.001; // Converted into GeV
    newJet.eta = l1jet->eta();
    newJet.phi = l1jet->phi();
    bool unique=true;
    for (auto & l1_jet : l1_jets) {
      if (newJet.pt  == l1_jet.pt &&
	  newJet.eta == l1_jet.eta &&
	  newJet.phi == l1_jet.phi) {
	unique = false;
	break;
      }
    }
    if(unique) l1_jets.push_back(newJet);
  }

  // RETRIEVE L1 JETS FOR JJ TRIGGERS
  std::vector<TrigBtagEmulationJet> l1_jets_jj;
  for (auto & l1jet : *l1JetContainer) {
    TrigBtagEmulationJet newJet;
    newJet.pt  = l1jet->et4x4()*0.001; // Converted into GeV   
    newJet.eta = l1jet->eta();
    newJet.phi = l1jet->phi();
    bool unique=true;
    for (auto & l1_jet : l1_jets_jj) {
      if (newJet.pt  == l1_jet.pt &&
          newJet.eta == l1_jet.eta &&
          newJet.phi == l1_jet.phi) {
        unique = false;
        break;
      }
    }
    if(unique) l1_jets_jj.push_back(newJet);
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

  StatusCode sc = StatusCode::SUCCESS;

  // jet Menagers
  m_manager_ef = new Trig::jetManager(m_trigDec,m_input_chain,m_input_jetName,m_input_btagName);
  m_manager_ef->setKeys(m_input_jetKey,m_input_pvKey,m_input_tpKey);
  m_manager_split = new Trig::jetManager(m_trigDec,m_input_chainSplit,m_input_jetNameSplit,m_input_btagName);
  m_manager_split->setKeys(m_input_jetKeySplit,m_input_pvKeySplit,m_input_tpKeySplit);
  m_manager_gsc = new  Trig::jetManager(m_trigDec,m_input_chainGSC,m_input_JetName_GSC,m_input_btagName); 
  m_manager_gsc->setKeys(m_input_jetKey_GSC,m_input_pvKey_GSC,m_input_tpKey_GSC);
  m_manager_ef_gsc = new Trig::jetManager(m_trigDec,"HLT_j15_boffperf",m_input_JetName_GSC,m_input_btagName);
  m_manager_ef_gsc->setKeys(m_input_jetKey,m_input_pvKey,m_input_tpKey);
  m_manager_split_gsc = new Trig::jetManager(m_trigDec,m_input_chainGSC,m_input_JetName_GSC,m_input_btagName);
  m_manager_split_gsc->setKeys(m_input_jetKeySplit,m_input_pvKeySplit,m_input_tpKeySplit);
  
  // GET EF JETS
  if (m_useTriggerNavigation) sc = m_manager_ef->retrieveByNavigation();
  else sc = m_manager_ef->retrieveByContainer( evtStore() );  
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Could not retrieve EF Jets!" );
    return sc;
  }
  else if (m_useTriggerNavigation) {
    if (m_verbosity > 0) 
      ATH_MSG_INFO( Form( "Size of input containers (%s): jet=%lu vtx==%lu trk=%lu btg=%lu",m_manager_ef->m_chain.c_str(),
			  m_manager_ef->m_jet_Containers.size(), m_manager_ef->m_primaryVertex_Containers.size(),
			  m_manager_ef->m_trackParticle_Containers.size(),m_manager_ef->m_btagging_Containers.size()) );
  } else {
    if (m_verbosity > 0)
      ATH_MSG_INFO( Form( "Size of input containers (%s) [%s,%s]: jet=%lu vtx==%lu trk=%lu btg=%lu",
			  m_manager_ef->m_chain.c_str(),m_manager_ef->m_jetContainer.c_str(),m_manager_ef->m_btagContainer.c_str(),
			  m_manager_ef->m_jet_Containers.size(), m_manager_ef->m_primaryVertex_Containers.size(),
			  m_manager_ef->m_trackParticle_Containers.size(),m_manager_ef->m_btagging_Containers.size()) );
  }
  
  // GET SPLIT JETS
  if (m_useTriggerNavigation) sc = m_manager_split->retrieveByNavigation();
  else sc = m_manager_split->retrieveByContainer( evtStore() );  
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Could not retrieve SPLIT Jets!" );
    return sc;
  } else if (m_useTriggerNavigation) {
    if (m_verbosity > 0) 
      ATH_MSG_INFO( Form( "Size of input containers (%s): jet=%lu vtx==%lu trk=%lu btg=%lu",m_manager_split->m_chain.c_str(),
			  m_manager_split->m_jet_Containers.size(), m_manager_split->m_primaryVertex_Containers.size(),
			  m_manager_split->m_trackParticle_Containers.size(),m_manager_split->m_btagging_Containers.size()) );
  } else {
    if (m_verbosity > 0)
      ATH_MSG_INFO( Form( "Size of input containers (%s) [%s,%s]: jet=%lu vtx==%lu trk=%lu btg=%lu",
			  m_manager_split->m_chain.c_str(),m_manager_split->m_jetContainer.c_str(),m_manager_split->m_btagContainer.c_str(),
			  m_manager_split->m_jet_Containers.size(), m_manager_split->m_primaryVertex_Containers.size(),
			  m_manager_split->m_trackParticle_Containers.size(),m_manager_split->m_btagging_Containers.size()) );
  }
  

  // GET GSC JETS
  if (this->hasGSC()) {
    if (m_useTriggerNavigation) sc = m_manager_gsc->retrieveByNavigation();
    else sc = m_manager_gsc->retrieveByContainer( evtStore() );  
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR( "Could not retrieve SPLIT Jets!" );
      return sc;
    } else if (m_useTriggerNavigation) {
      if (m_verbosity > 0) 
	ATH_MSG_INFO( Form( "Size of input containers (%s): jet=%lu vtx==%lu trk=%lu btg=%lu",m_manager_gsc->m_chain.c_str(),
			    m_manager_gsc->m_jet_Containers.size(), m_manager_gsc->m_primaryVertex_Containers.size(),
			    m_manager_gsc->m_trackParticle_Containers.size(),m_manager_gsc->m_btagging_Containers.size()) );
    } else {
      if (m_verbosity > 0)
	ATH_MSG_INFO( Form( "Size of input containers (%s) [%s,%s]: jet=%lu vtx==%lu trk=%lu btg=%lu",
			    m_manager_gsc->m_chain.c_str(),m_manager_gsc->m_jetContainer.c_str(),m_manager_gsc->m_btagContainer.c_str(),
			    m_manager_gsc->m_jet_Containers.size(), m_manager_gsc->m_primaryVertex_Containers.size(),
			    m_manager_gsc->m_trackParticle_Containers.size(),m_manager_gsc->m_btagging_Containers.size()) );
    }
  }
  
  // GET EF JETS FOR GSC TRIGGERS
  if (this->hasGSC()) {
    if (m_useTriggerNavigation) sc = m_manager_ef_gsc->retrieveByNavigation();
    else sc = m_manager_ef_gsc->retrieveByContainer( evtStore() );  
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR( "Could not retrieve EF Jets for GSC triggers!" );
      return sc;
    } else if (m_useTriggerNavigation) {
      if (m_verbosity > 0) 
	ATH_MSG_INFO( Form( "Size of input containers (%s): jet=%lu vtx==%lu trk=%lu btg=%lu",m_manager_ef_gsc->m_chain.c_str(),
			    m_manager_ef_gsc->m_jet_Containers.size(), m_manager_ef_gsc->m_primaryVertex_Containers.size(),
			    m_manager_ef_gsc->m_trackParticle_Containers.size(),m_manager_ef_gsc->m_btagging_Containers.size()) );
    } else {
      if (m_verbosity > 0)
	ATH_MSG_INFO( Form( "Size of input containers (%s) [%s,%s]: jet=%lu vtx==%lu trk=%lu btg=%lu",
			    m_manager_ef_gsc->m_chain.c_str(),m_manager_ef_gsc->m_jetContainer.c_str(),m_manager_ef_gsc->m_btagContainer.c_str(),
			    m_manager_ef_gsc->m_jet_Containers.size(), m_manager_ef_gsc->m_primaryVertex_Containers.size(),
			    m_manager_ef_gsc->m_trackParticle_Containers.size(),m_manager_ef_gsc->m_btagging_Containers.size()) );
    }
  }
  
  // GET SPLIT JETS FOR GSC TRIGGERS
  if (this->hasGSC()) {
    if (m_useTriggerNavigation) sc = m_manager_split_gsc->retrieveByNavigation();
    else sc = m_manager_split_gsc->retrieveByContainer( evtStore() );  
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR( "Could not retrieve SPLIT Jets for GSC triggers!" );
      return sc;
    } else if (m_useTriggerNavigation) {
      if (m_verbosity > 0) 
	ATH_MSG_INFO( Form( "Size of input containers (%s): jet=%lu vtx==%lu trk=%lu btg=%lu",m_manager_split_gsc->m_chain.c_str(),
			    m_manager_split_gsc->m_jet_Containers.size(), m_manager_split_gsc->m_primaryVertex_Containers.size(),
			    m_manager_split_gsc->m_trackParticle_Containers.size(),m_manager_split_gsc->m_btagging_Containers.size()) );
    } else {
      if (m_verbosity > 0)
	ATH_MSG_INFO( Form( "Size of input containers (%s) [%s,%s]: jet=%lu vtx==%lu trk=%lu btg=%lu",
			    m_manager_split_gsc->m_chain.c_str(),m_manager_split_gsc->m_jetContainer.c_str(),m_manager_split_gsc->m_btagContainer.c_str(),
			    m_manager_split_gsc->m_jet_Containers.size(), m_manager_split_gsc->m_primaryVertex_Containers.size(),
			    m_manager_split_gsc->m_trackParticle_Containers.size(),m_manager_split_gsc->m_btagging_Containers.size()) );
    }           
  }
  
  
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
  m_manager_ef->merge(input_jetsHT,0,35000);
  m_manager_split->merge(input_jetsHT,0,35000);

  *m_manager_ef_gsc += *m_manager_split_gsc;
  *m_manager_split_gsc += *m_manager_ef_gsc;
  m_manager_ef_gsc->merge(input_jetsHT,0,35000);
  m_manager_split_gsc->merge(input_jetsHT,0,35000);

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
      ATH_MSG_INFO ("  Jet --- pt[et8x8]=" << l1_jet.pt << " pt[et4x4]="<< l1_jet_jj.pt  << " eta=" << l1_jet.eta << " phi=" << l1_jet.phi );
    }

    // DUMP EF JETS
    ATH_MSG_INFO ("EF jets");
    for (auto & jet : m_manager_ef->getJets()) {
      ATH_MSG_INFO ("  Jet --- pt=" << jet.pt*1e-3 << " eta=" << jet.eta << " phi=" << jet.phi);
      for (auto & weight : jet.weights) 
	ATH_MSG_INFO ("      " << weight.first << " " << weight.second);
    }
    
    // DUMP SPLIT JETS
    ATH_MSG_INFO ("SPLIT jets");
    for (auto & jet : m_manager_split->getJets()) {
      ATH_MSG_INFO ("  Jet --- pt=" << jet.pt*1e-3 << " eta=" << jet.eta << " phi=" << jet.phi);
      for (auto & weight : jet.weights)
	ATH_MSG_INFO ("      " << weight.first << " " << weight.second);
    }
    
    // DUMP GSC JETS
    ATH_MSG_INFO ("GSC jets");
    for (auto & jet : m_manager_gsc->getJets()) {
      ATH_MSG_INFO ("  Jet --- pt=" << jet.pt*1e-3 << " eta=" << jet.eta);
      for (auto & weight : jet.weights)
	ATH_MSG_INFO ("      " << weight.first << " " << weight.second);
    }
    
    // DUMP EMULATED DECISIONS
    ATH_MSG_INFO ("Emulated decisions");
    for (auto & emulatedChain : m_emulatedChains) {
      if (m_verbosity > 1) emulatedChain.second.Print();
      ATH_MSG_INFO ("  Chain --- name=" << emulatedChain.first << " result=" << (int)emulatedChain.second.isPassed());
    }
    
  }
  
  delete m_manager_ef;
  delete m_manager_split;
  delete m_manager_gsc;
  delete m_manager_ef_gsc;
  delete m_manager_split_gsc;

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
  const char* inputFileFolder = gSystem->ExpandPathName ("${WorkDir_DIR}");
  // If WorkDir_DIR not defined means we are using ROOTCORE  
  if( strlen(inputFileFolder) == 0 ) 
    inputFileFolder = gSystem->ExpandPathName ("${ROOTCOREBIN}");

  std::string nameFile_2015 = Form("%s/data/TrigBtagEmulationTool/triggerChains_2015Menu.txt",inputFileFolder);
  std::string nameFile_2016 = Form("%s/data/TrigBtagEmulationTool/triggerChains_2016Menu.txt",inputFileFolder);
  std::string nameFile_2017 = Form("%s/data/TrigBtagEmulationTool/triggerChains_2017Menu.txt",inputFileFolder);

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
  for (const auto &sgj : *sgJetContainer)
    {
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
