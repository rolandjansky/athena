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
 *
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration 
 **********************************************************************/

#include "TrigBtagEmulationTool/TrigBtagEmulationTool.h"

#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "TSystem.h"

// FrameWork includes
#ifndef ROOTCORE
#include "GaudiKernel/Property.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#else
#include "TrigConfxAOD/xAODConfigTool.h"
#include <AsgTools/MessageCheck.h>
#endif

#include "fstream"

using namespace Trig;

//**********************************************************************

TrigBtagEmulationChain::TrigBtagEmulationChain(const std::vector<std::string>& chainDefinition, ToolHandle<Trig::TrigDecisionTool>& trigDec) 
  : m_trigDec(trigDec), 
    m_jetCount(0), 
    m_bjetCount(0) {

  // Prepare loop on ingredients
  std::vector<std::string>::const_iterator name=chainDefinition.begin(), nameEnd=chainDefinition.end();

  // Extract first entry = name
  m_name = *name;
  name++;

  // Extract basic configuration info fron name (configuration and tagger)
  if(m_name.find("split") != std::string::npos ) {
    m_bjetConfig = "SPLIT";
  } else {
    m_bjetConfig = "NONSPLIT";
  }

  if(m_name.find("mv2c10") != std::string::npos) 
    m_bjetTagger = "MV2c10";
  else if(m_name.find("mv2c20") != std::string::npos) 
    m_bjetTagger = "MV2c20";
  else 
    m_bjetTagger = "COMB";
  

  // Loop on ingredents
  for( ; name!=nameEnd; name++) {

    // Handle emulated L1
    if(name->find("EMUL_L1")!=std::string::npos) {
      float min_pt, min_eta=0.0, max_eta=3.1;
      unsigned int min_mult=1;
      float min_ht = 0;
      unsigned int topEt = 0;
      float min_invm = 0;
      bool isJJ = false;
      bool isCF = false;

      // Loop on tokens
      std::istringstream sname(*name);
      std::string token;
      while(std::getline(sname, token, '_')) {
	// Loop on subtokens
	if(token.find("J")!=std::string::npos) {
	  std::istringstream ssname(token);
	  std::string stoken;

	  if (token.find("HT")!=std::string::npos)
            {
	      std::getline(ssname, stoken, '-');
	      std::istringstream s_ht(stoken.substr(stoken.find("HT")+2,stoken.length()-stoken.find("HT")-2));
              s_ht >> min_ht;
            }

	  while(std::getline(ssname, stoken, '.')) {

	    if (stoken.find("MJJ")!=std::string::npos)
	      {
		// Change eta so that the whole range is considered in the computation
		max_eta = 4.9;
		std::istringstream s_min_invm(stoken.substr(stoken.find("MJJ-")+4,stoken.length()-stoken.find("MJJ-")-4));
                s_min_invm >> min_invm;
		if (stoken.find("-CF")!=std::string::npos) isCF = true;
	      }
	    else if (stoken.find("JJ")!=std::string::npos)
	      {
		std::istringstream s_min_pt(stoken.substr(stoken.find("JJ-")+3,stoken.length()-stoken.find("JJ-")-3));
                s_min_pt >> min_pt;
		isJJ = true;
	      }
	    // pt and mult for L1
	    else if(stoken.find("J")!=std::string::npos) {
              // Jxxx                                                                                                                                            
              if(stoken.find("s")==std::string::npos) {
		std::istringstream s_min_pt(stoken.substr(stoken.find("J")+1,stoken.length()-stoken.find("J")-1));
                s_min_pt >> min_pt;
              } else {
		std::istringstream s_topEt(stoken.substr(stoken.find("s")+1,stoken.length()-stoken.find("s")-1));
		std::istringstream s_min_pt(stoken.substr(stoken.find("J")+1,stoken.find("s")-stoken.find("J")-1));
                s_topEt >> topEt;
                s_min_pt >> min_pt;
              }
              // xxxJ                                                                                                                                            
              if(stoken.find("J")>0) {
		std::istringstream s_min_mult(stoken.substr(0,stoken.find("J")));
                s_min_mult >> min_mult;
              }
            }

	    // eta for L1
	    else if(stoken.find("ETA")!=std::string::npos) {
	      std::istringstream s_min_eta(stoken.substr(0,stoken.find("E")));
	      s_min_eta >> min_eta; min_eta /= 10.0;
	      std::istringstream s_max_eta(stoken.substr(stoken.find("A")+1,stoken.length()-stoken.find("A")-1));
	      s_max_eta >> max_eta; max_eta /= 10.0;
	    }
	    
	    

	  }
	}
      }
      if (isJJ) addL1JJJetIngredient(sname.str(),min_pt, min_eta, max_eta, min_mult, min_ht,topEt,min_invm,isCF);
      else addL1JetIngredient(sname.str(),min_pt, min_eta, max_eta, min_mult, min_ht,topEt,min_invm,isCF);
    }
    // Handle emulated HLT
    else if(name->find("EMUL_HLT")!=std::string::npos) {
      float min_pt = 0., min_eta=0.0, max_eta=3.2;
      float min_weight = -1000, anti_weight = 1000;
      unsigned int min_mult=1;
      float min_ht = 0;
      float min_gsc = 0;
      float min_invm = 0;
      bool bjet=false;

      // Loop on tokens
      std::istringstream sname(*name);
      std::string token;
      while(std::getline(sname, token, '_')) {
	if(token.find("ht")!=std::string::npos) {
	  std::istringstream s_min_ht(token.substr(token.find("ht")+2,token.length()-token.find("ht")-2));
	  s_min_ht >> min_ht;
	}

	if (token.find("gsc")!=std::string::npos) {
	  std::istringstream s_min_gsc(token.substr(token.find("gsc")+3,token.length()-token.find("gsc")-3));
          s_min_gsc >> min_gsc;
        }
	
	if (token.find("invm")!=std::string::npos) {
	  std::istringstream s_min_invm(token.substr(token.find("invm")+4,token.length()-token.find("invm")-4));
	  s_min_invm >> min_invm;
	}

	// pt and mult for HLT
	if(token.find("j")!=std::string::npos) {
	  std::istringstream s_min_pt(token.substr(token.find("j")+1,token.length()-token.find("j")-1));
	  s_min_pt >> min_pt;
	  if(token.find("j")>0) {
	    std::istringstream s_min_mult(token.substr(0,token.find("j")));
	    s_min_mult >> min_mult;
	  }
	}
	// eta for HLT
	if(token.find("eta")!=std::string::npos) {
	  std::istringstream s_min_eta(token.substr(0,token.find("e")));
	  s_min_eta >> min_eta; min_eta /= 100.0;
	  std::istringstream s_max_eta(token.substr(token.find("a")+1,token.length()-token.find("a")-1));
	  s_max_eta >> max_eta; max_eta /= 100.0;
	}
	// weight for HLT
	if(token.find("b")!=std::string::npos) {
	  bjet=true;
	  double theWeight = -1000;

	  if(token.find("bcombloose")!=std::string::npos) theWeight=0.25;
	  else if(token.find("bcombmedium")!=std::string::npos) theWeight=1.25;
          else if(token.find("bcombtight")!=std::string::npos) theWeight=2.65;
	  else if(token.find("bloose")!=std::string::npos) theWeight=0.25;
	  else if(token.find("bmedium")!=std::string::npos) theWeight=1.25;
	  else if(token.find("btight")!=std::string::npos) theWeight=2.65;
	  else if(token.find("mv2c2040")!=std::string::npos) theWeight=0.75;
	  else if(token.find("mv2c2050")!=std::string::npos) theWeight=0.5;
	  else if(token.find("mv2c2060")!=std::string::npos) theWeight=-0.0224729;
	  else if(token.find("mv2c2070")!=std::string::npos) theWeight=-0.509032;
	  else if(token.find("mv2c2077")!=std::string::npos) theWeight=-0.764668;
	  else if(token.find("mv2c2085")!=std::string::npos) theWeight=-0.938441;
	  // mv2c10 cuts: https://indico.cern.ch/event/642743/contributions/2612294/attachments/1468496/2271441/BJetTrigTuning_TGM31May2017.pdf 
	  else if(token.find("mv2c1040")!=std::string::npos) theWeight=0.978;
          else if(token.find("mv2c1050")!=std::string::npos) theWeight=0.948;
          else if(token.find("mv2c1060")!=std::string::npos) theWeight=0.846;
          else if(token.find("mv2c1070")!=std::string::npos) theWeight=0.58;
          else if(token.find("mv2c1077")!=std::string::npos) theWeight=0.162;
          else if(token.find("mv2c1085")!=std::string::npos) theWeight=-0.494;

	  if (token.find("ANTI")==std::string::npos) min_weight = theWeight;
	  else if (theWeight != -1000) anti_weight = theWeight;
	}

      }
      if(!bjet) addHLTJetIngredient(sname.str(),min_pt*1000.0, min_eta, max_eta, min_mult, min_ht*1000, min_gsc*1000, min_invm*1000); //TODO Use GeV
      else addHLTBjetIngredient(sname.str(),min_pt*1000.0, min_eta, max_eta, m_bjetTagger, min_weight, min_mult, min_ht*1000, min_gsc*1000, anti_weight, min_invm*1000); 
    }
    // Handle already available L1 or HLT decision
    else if((name->find("L1_")!=std::string::npos) || (name->find("HLT_")!=std::string::npos)) {
      addDecisionIngredient(*name);
    }

  }
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
#ifndef ROOTCORE
    m_storeGate("StoreGateSvc", name),
    m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
    m_dsSvc("TrigConf::DSConfigSvc/DSConfigSvc", name),
    m_bTagTool("Analysis::BTagTool"),
    m_bTagTrackAssocTool("Analysis::BTagTrackAssociation"),
    m_bTagSecVtxTool("Analysis::BTagSecVertexing"),
#else
    m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc"),
    m_dsSvc("TrigConf::DSConfigSvc/DSConfigSvc"),
#endif
    manager_ef(nullptr),
    manager_split(nullptr),
    manager_gsc(nullptr),
    manager_ef_gsc(nullptr),
    manager_split_gsc(nullptr),
    m_splitTrigger(false),
    m_btagTrigger(false),
    m_htTrigger(false),
    m_gscTrigger(false)
{

#ifndef ROOTCORE
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

#ifdef ROOTCORE
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

#ifndef ROOTCORE
  sc = m_storeGate.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Could not retrieve StoreGateSvc!" << endreq;
    return sc;
  }

  // Retrieve the trigger configuration service
  sc = m_configSvc.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Could not retrieve Trigger configuration!" << endreq;
    return sc;
  }

  std::cout<<"Retrieving TrigDecisionTool"<<std::endl;
  // Retrieve TrigDecisionTool
  sc = m_trigDec.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Could not retrieve TrigDecisionTool!" << endreq;
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
      ANA_CHECK(m_trigDec->setProperty( "ConfigTool", trigConfigHandle ) );
      ANA_CHECK(m_trigDec->setProperty( "TrigDecisionKey", "xTrigDecision" ) );
      ANA_CHECK(m_trigDec->initialize() );
    }

#endif


#ifndef ROOTCORE
  // RETRIEVE OFFLINE TOOLS
  // Retrieve the offline track association tool
  if (!m_bTagTrackAssocTool.empty()) {
    if (m_bTagTrackAssocTool.retrieve().isFailure()) {
      msg() << MSG::FATAL << "Failed to locate tool " << m_bTagTrackAssocTool << endreq;
      return StatusCode::FAILURE;
    } else
      msg() << MSG::INFO << "Retrieved tool " << m_bTagTrackAssocTool << endreq;
  } else
    msg() << MSG::DEBUG << "No track association tool to retrieve" << endreq;
  // Retrieve the bTagSecVtxTool
  if (m_bTagSecVtxTool.retrieve().isFailure()) {
    msg() << MSG::FATAL << "Failed to locate tool " << m_bTagSecVtxTool << endreq;
    return StatusCode::FAILURE;
  } else
    msg() << MSG::INFO << "Retrieved tool " << m_bTagSecVtxTool << endreq;
  // Retrieve the main BTagTool
  if (m_bTagTool.retrieve().isFailure()) {
    msg() << MSG::FATAL << "Failed to locate tool " << m_bTagTool << endreq;
    return StatusCode::FAILURE;
  } else
    msg() << MSG::INFO << "Retrieved tool " << m_bTagTool << endreq;
#endif

  // CREATE EMULATED CHAINS
  for (auto definition : m_emulatedChainDefinitions) {
    checkTriggerChain(definition);

    TrigBtagEmulationChain chain(TrigBtagEmulationChain(definition,m_trigDec));
    m_emulatedChains.insert(std::make_pair(chain.name(),chain));
  }

#ifndef ROOTCORE
  jetManager::m_bTagTool = &m_bTagTool;
  jetManager::m_bTagTrackAssocTool = &m_bTagTrackAssocTool;
  jetManager::m_bTagSecVtxTool = &m_bTagSecVtxTool;
#endif

  // Autoconfiguration of Trigger Menu
  if (this->initTriggerChainsMenu().isFailure() )
    {
      msg(MSG::ERROR) << "Could not initialize lowest unprescaled Trigger Chains!" << endreq;
      return StatusCode::FAILURE;
    }
  if (m_autoconfiguredMenu == "2015Menu" || m_autoconfiguredMenu == "2016Menu" || m_autoconfiguredMenu == "2015+2016Menu")
    {
      ATH_MSG_INFO( Form( "Automatic configuration of Trigger Chains for %s",m_autoconfiguredMenu.c_str()) );
      ATH_MSG_INFO( Form( "For full list of trigger chains automatically loaded look here : '%s'","https://twiki.cern.ch/twiki/bin/view/Atlas/TrigBjetEmulation") );
      this->addEmulatedChain( m_autoconfiguredMenu );
    }

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

#ifndef ROOTCORE
  CHECK( evtStore()->retrieve(l1JetContainer,"LVL1JetRoIs") );
#else
  ANA_CHECK( evtStore()->retrieve(l1JetContainer,"LVL1JetRoIs") );
#endif

  for (auto & l1jet : *l1JetContainer) {
    TrigBtagEmulationJet newJet;
    newJet.pt  = l1jet->et8x8()*0.001;
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
    newJet.pt  = l1jet->et4x4()*0.001;
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
    msg(MSG::ERROR) << "Could not retrieve Input Containers!" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_verbosity > 0) ATH_MSG_INFO ("Size of input HT containers (" << HTContainer << "):" << " jet=" << input_jetsHT.size());

  StatusCode sc = StatusCode::SUCCESS;

  // jet Menagers
  manager_ef = new Trig::jetManager(m_trigDec,m_input_chain,m_input_jetName,m_input_btagName);
  manager_ef->setKeys(m_input_jetKey,m_input_pvKey,m_input_tpKey);
  manager_split = new Trig::jetManager(m_trigDec,m_input_chainSplit,m_input_jetNameSplit,m_input_btagName);
  manager_split->setKeys(m_input_jetKeySplit,m_input_pvKeySplit,m_input_tpKeySplit);
  manager_gsc = new  Trig::jetManager(m_trigDec,m_input_chainGSC,m_input_JetName_GSC,m_input_btagName); 
  manager_gsc->setKeys(m_input_jetKey_GSC,m_input_pvKey_GSC,m_input_tpKey_GSC);
  manager_ef_gsc = new Trig::jetManager(m_trigDec,"HLT_j15_boffperf",m_input_JetName_GSC,m_input_btagName);
  manager_ef_gsc->setKeys(m_input_jetKey,m_input_pvKey,m_input_tpKey);
  manager_split_gsc = new Trig::jetManager(m_trigDec,m_input_chainGSC,m_input_JetName_GSC,m_input_btagName);
  manager_split_gsc->setKeys(m_input_jetKeySplit,m_input_pvKeySplit,m_input_tpKeySplit);
  
  // GET EF JETS
  if (m_useTriggerNavigation) sc = manager_ef->retrieveByNavigation();
  else sc = manager_ef->retrieveByContainer( evtStore() );  
  if ( sc.isFailure() )
    {
      msg(MSG::ERROR) << "Could not retrieve EF Jets!" << endreq;
      return sc;
    }
  else if (m_useTriggerNavigation)
    {
      if (m_verbosity > 0) 
	ATH_MSG_INFO( Form( "Size of input containers (%s): jet=%lu vtx==%lu trk=%lu btg=%lu",manager_ef->_chain.c_str(),
			    manager_ef->_jet_Containers.size(), manager_ef->_primaryVertex_Containers.size(),
			    manager_ef->_trackParticle_Containers.size(),manager_ef->_btagging_Containers.size()) );
    }
  else 
    {
      if (m_verbosity > 0)
	ATH_MSG_INFO( Form( "Size of input containers (%s) [%s,%s]: jet=%lu vtx==%lu trk=%lu btg=%lu",
			    manager_ef->_chain.c_str(),manager_ef->_jetContainer.c_str(),manager_ef->_btagContainer.c_str(),
			    manager_ef->_jet_Containers.size(), manager_ef->_primaryVertex_Containers.size(),
			    manager_ef->_trackParticle_Containers.size(),manager_ef->_btagging_Containers.size()) );
    }

  // GET SPLIT JETS
  if (m_useTriggerNavigation) sc = manager_split->retrieveByNavigation();
  else sc = manager_split->retrieveByContainer( evtStore() );  
  if ( sc.isFailure() )    
    {
      msg(MSG::ERROR) << "Could not retrieve SPLIT Jets!" << endreq;
      return sc;
    }
  else if (m_useTriggerNavigation)
    {
      if (m_verbosity > 0) 
	ATH_MSG_INFO( Form( "Size of input containers (%s): jet=%lu vtx==%lu trk=%lu btg=%lu",manager_split->_chain.c_str(),
			    manager_split->_jet_Containers.size(), manager_split->_primaryVertex_Containers.size(),
			    manager_split->_trackParticle_Containers.size(),manager_split->_btagging_Containers.size()) );
    }
  else 
    {
      if (m_verbosity > 0)
	ATH_MSG_INFO( Form( "Size of input containers (%s) [%s,%s]: jet=%lu vtx==%lu trk=%lu btg=%lu",
			    manager_split->_chain.c_str(),manager_split->_jetContainer.c_str(),manager_split->_btagContainer.c_str(),
			    manager_split->_jet_Containers.size(), manager_split->_primaryVertex_Containers.size(),
			    manager_split->_trackParticle_Containers.size(),manager_split->_btagging_Containers.size()) );
    }


  // GET GSC JETS
  if (this->hasGSC())
    {
      if (m_useTriggerNavigation) sc = manager_gsc->retrieveByNavigation();
      else sc = manager_gsc->retrieveByContainer( evtStore() );  
      if ( sc.isFailure() )
	{
	  msg(MSG::ERROR) << "Could not retrieve GSC Jets!" << endreq;
	  return sc;
	}
      else if (m_useTriggerNavigation)
	{
	  if (m_verbosity > 0) 
	    ATH_MSG_INFO( Form( "Size of input containers (%s): jet=%lu vtx==%lu trk=%lu btg=%lu",manager_gsc->_chain.c_str(),
				manager_gsc->_jet_Containers.size(), manager_gsc->_primaryVertex_Containers.size(),
				manager_gsc->_trackParticle_Containers.size(),manager_gsc->_btagging_Containers.size()) );
	}
      else 
	{
	  if (m_verbosity > 0)
	    ATH_MSG_INFO( Form( "Size of input containers (%s) [%s,%s]: jet=%lu vtx==%lu trk=%lu btg=%lu",
				manager_gsc->_chain.c_str(),manager_gsc->_jetContainer.c_str(),manager_gsc->_btagContainer.c_str(),
				manager_gsc->_jet_Containers.size(), manager_gsc->_primaryVertex_Containers.size(),
				manager_gsc->_trackParticle_Containers.size(),manager_gsc->_btagging_Containers.size()) );
	}
    }

  // GET EF JETS FOR GSC TRIGGERS
  if (this->hasGSC())
    {
      if (m_useTriggerNavigation) sc = manager_ef_gsc->retrieveByNavigation();
      else sc = manager_ef_gsc->retrieveByContainer( evtStore() );  
      if ( sc.isFailure() )
	{
	  msg(MSG::ERROR) << "Could not retrieve EF Jets for GSC triggers!" << endreq;
	  return sc;
	}
      else if (m_useTriggerNavigation)
	{
	  if (m_verbosity > 0) 
	    ATH_MSG_INFO( Form( "Size of input containers (%s): jet=%lu vtx==%lu trk=%lu btg=%lu",manager_ef_gsc->_chain.c_str(),
				manager_ef_gsc->_jet_Containers.size(), manager_ef_gsc->_primaryVertex_Containers.size(),
				manager_ef_gsc->_trackParticle_Containers.size(),manager_ef_gsc->_btagging_Containers.size()) );
	}
      else 
	{
	  if (m_verbosity > 0)
	    ATH_MSG_INFO( Form( "Size of input containers (%s) [%s,%s]: jet=%lu vtx==%lu trk=%lu btg=%lu",
				manager_ef_gsc->_chain.c_str(),manager_ef_gsc->_jetContainer.c_str(),manager_ef_gsc->_btagContainer.c_str(),
				manager_ef_gsc->_jet_Containers.size(), manager_ef_gsc->_primaryVertex_Containers.size(),
				manager_ef_gsc->_trackParticle_Containers.size(),manager_ef_gsc->_btagging_Containers.size()) );
	}
    }

  // GET SPLIT JETS FOR GSC TRIGGERS
  if (this->hasGSC())
    {
      if (m_useTriggerNavigation) sc = manager_split_gsc->retrieveByNavigation();
      else sc = manager_split_gsc->retrieveByContainer( evtStore() );  
      if ( sc.isFailure() )
	{
	  msg(MSG::ERROR) << "Could not retrieve SPLIT Jets for GSC triggers!" << endreq;
	  return sc;
	}
      else if (m_useTriggerNavigation)
	{
	  if (m_verbosity > 0) 
	    ATH_MSG_INFO( Form( "Size of input containers (%s): jet=%lu vtx==%lu trk=%lu btg=%lu",manager_split_gsc->_chain.c_str(),
				manager_split_gsc->_jet_Containers.size(), manager_split_gsc->_primaryVertex_Containers.size(),
				manager_split_gsc->_trackParticle_Containers.size(),manager_split_gsc->_btagging_Containers.size()) );
	}
      else 
	{
	  if (m_verbosity > 0)
	    ATH_MSG_INFO( Form( "Size of input containers (%s) [%s,%s]: jet=%lu vtx==%lu trk=%lu btg=%lu",
				manager_split_gsc->_chain.c_str(),manager_split_gsc->_jetContainer.c_str(),manager_split_gsc->_btagContainer.c_str(),
				manager_split_gsc->_jet_Containers.size(), manager_split_gsc->_primaryVertex_Containers.size(),
				manager_split_gsc->_trackParticle_Containers.size(),manager_split_gsc->_btagging_Containers.size()) );
	}           
    }
  

  // RETAG COPYING ORIGINAL WEIGHTS 
  if (!m_useTriggerNavigation || !m_tagOfflineWeights || !m_tagOnlineWeights)
    {
#ifndef ROOTCORE
      CHECK( manager_ef->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
      CHECK( manager_split->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
      CHECK( manager_gsc->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
      CHECK( manager_ef_gsc->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
      CHECK( manager_split_gsc->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
#else
      ANA_CHECK( manager_ef->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
      ANA_CHECK( manager_split->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
      ANA_CHECK( manager_gsc->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
      ANA_CHECK( manager_ef_gsc->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
      ANA_CHECK( manager_split_gsc->retagCopy(m_useTriggerNavigation,m_tagOfflineWeights,m_tagOnlineWeights) );
#endif      
    }
  // RETAG WITH OFFLINE TOOLS    
  if (m_useTriggerNavigation && m_tagOfflineWeights)
    {
#ifndef ROOTCORE
      CHECK( manager_ef->retagOffline()        );
      CHECK( manager_split->retagOffline()     );
      CHECK( manager_gsc->retagOffline()       );
      CHECK( manager_ef_gsc->retagOffline()    );
      CHECK( manager_split_gsc->retagOffline() );
#else
      ANA_CHECK( manager_ef->retagOffline()        );
      ANA_CHECK( manager_split->retagOffline()     );
      ANA_CHECK( manager_gsc->retagOffline()       );
      ANA_CHECK( manager_ef_gsc->retagOffline()    );
      ANA_CHECK( manager_split_gsc->retagOffline() );
#endif
    }
  // RETAG WITH ONLINE TOOLS
  if (m_useTriggerNavigation && m_tagOnlineWeights)
    {
#ifndef ROOTCORE
      CHECK( manager_ef->retagOnline()        );
      CHECK( manager_split->retagOnline()     );
      CHECK( manager_gsc->retagOnline()       );
      CHECK( manager_ef_gsc->retagOnline()    );
      CHECK( manager_split_gsc->retagOnline() );
#else
      ANA_CHECK( manager_ef->retagOnline()        );
      ANA_CHECK( manager_split->retagOnline()     );
      ANA_CHECK( manager_gsc->retagOnline()       );
      ANA_CHECK( manager_ef_gsc->retagOnline()    );
      ANA_CHECK( manager_split_gsc->retagOnline() );
#endif
    }

  // BACKUP NON-CENTRAL JET INFO FOR SPLIT CHAINS  
  *manager_ef += *manager_split;
  *manager_split += *manager_ef;
  manager_ef->merge(input_jetsHT,0,35000);
  manager_split->merge(input_jetsHT,0,35000);

  *manager_ef_gsc += *manager_split_gsc;
  *manager_split_gsc += *manager_ef_gsc;
  manager_ef_gsc->merge(input_jetsHT,0,35000);
  manager_split_gsc->merge(input_jetsHT,0,35000);

  // EVALUATE EMULATED CHAINS
  for (auto & emulatedChain : m_emulatedChains) {
    emulatedChain.second.addJet("LVL1",l1_jets);
    emulatedChain.second.addJet("LVL1_JJ",l1_jets_jj);
    emulatedChain.second.addJet("EF",manager_ef->getJets());
    emulatedChain.second.addJet("SPLIT",manager_split->getJets());
    emulatedChain.second.addJet("GSC",manager_gsc->getJets());
    emulatedChain.second.addJet("EF_GSC",manager_ef_gsc->getJets());
    emulatedChain.second.addJet("SPLIT_GSC",manager_split_gsc->getJets());

    emulatedChain.second.evaluate();
  }

  if (m_verbosity > 0)
    {
      // DUMP L1 JETS
      ATH_MSG_INFO ("L1 jets");
      for (unsigned int i=0; i<l1_jets.size(); i++)
	{
	  auto & l1_jet    = l1_jets.at(i);
	  auto & l1_jet_jj = l1_jets_jj.at(i);
	  ATH_MSG_INFO ("  Jet --- pt[et8x8]=" << l1_jet.pt << " pt[et4x4]="<< l1_jet_jj.pt  << " eta=" << l1_jet.eta << " phi=" << l1_jet.phi );
	}

      // DUMP EF JETS
      ATH_MSG_INFO ("EF jets");
      for (auto & jet : manager_ef->getJets()) {
	ATH_MSG_INFO ("  Jet --- pt=" << jet.pt*1e-3 << " eta=" << jet.eta << " phi=" << jet.phi);
	for (auto & weight : jet.weights) 
	  ATH_MSG_INFO ("      " << weight.first << " " << weight.second);
      }

      // DUMP SPLIT JETS
      ATH_MSG_INFO ("SPLIT jets");
      for (auto & jet : manager_split->getJets()) {
	ATH_MSG_INFO ("  Jet --- pt=" << jet.pt*1e-3 << " eta=" << jet.eta << " phi=" << jet.phi);
	for (auto & weight : jet.weights)
          ATH_MSG_INFO ("      " << weight.first << " " << weight.second);
      }

      // DUMP GSC JETS
      ATH_MSG_INFO ("GSC jets");
      for (auto & jet : manager_gsc->getJets()) {
	ATH_MSG_INFO ("  Jet --- pt=" << jet.pt*1e-3 << " eta=" << jet.eta);
        for (auto & weight : jet.weights)
          ATH_MSG_INFO ("      " << weight.first << " " << weight.second);
      }

      // DUMP EMULATED DECISIONS
      ATH_MSG_INFO ("Emulated decisions");
      for (auto & emulatedChain : m_emulatedChains) 
	{
	  if (m_verbosity > 1) emulatedChain.second.Print();
	  ATH_MSG_INFO ("  Chain --- name=" << emulatedChain.first << " result=" << (int)emulatedChain.second.isPassed());
	}

    }

  delete manager_ef;
  delete manager_split;
  delete manager_gsc;
  delete manager_ef_gsc;
  delete manager_split_gsc;

  return StatusCode::SUCCESS;

}


//!==========================================================================
StatusCode TrigBtagEmulationTool::finalize() {
  
  ATH_MSG_INFO( "Finalising tool " << name() );
  
  return StatusCode::SUCCESS;
}


//!==========================================================================
void TrigBtagEmulationTool::addEmulatedChain(const std::vector<std::string>& chainDescription) {

  // check if gsc chains are present in the emulation
  checkTriggerChain(chainDescription);

  // Create chain and add it to the list
  TrigBtagEmulationChain chain(TrigBtagEmulationChain(chainDescription,m_trigDec));
  m_emulatedChains.insert(std::make_pair(chain.name(),chain));

}

//!==========================================================================        NEW

StatusCode TrigBtagEmulationTool::initTriggerChainsMenu()
{
#ifdef ROOTCORE
  const char* inputFileFolder = gSystem->ExpandPathName ("${ROOTCOREBIN}");
#else
  const char* inputFileFolder = gSystem->ExpandPathName ("${WorkDir_DIR}");
#endif

  std::string nameFile_2015 = Form("%s/data/TrigBtagEmulationTool/triggerChains_2015Menu.txt",inputFileFolder);
  std::string nameFile_2016 = Form("%s/data/TrigBtagEmulationTool/triggerChains_2016Menu.txt",inputFileFolder);

  // *** 2015 Chains
  std::ifstream file2015; file2015.open( nameFile_2015.c_str() );
  if (!file2015)
    {
      msg(MSG::ERROR) << "Could not load lowest unprescaled Trigger Chains for 2015 !" << endreq;
      return StatusCode::FAILURE;
    }
  while ( !file2015.eof() )
    {
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
  if (!file2016)
    {
      msg(MSG::ERROR) << "Could not load lowest unprescaled Trigger Chains for 2016 !" << endreq;
      return StatusCode::FAILURE;
    }
  while ( !file2016.eof() )
    {
      std::string line;
      getline (file2016,line);
      if (line.empty()) continue;
      std::istringstream is_line(line);
      std::string chainName, chainComponents;
      is_line >> chainName >> chainComponents;
      m_2016Menu[chainName] = chainComponents; 
    }
  file2016.close();
  return StatusCode::SUCCESS;
}

std::vector<std::string> TrigBtagEmulationTool::addEmulatedChain(const std::string triggerMenu) {

  std::map< std::string,std::string > configuration;
  if ( triggerMenu.find("2015")!=std::string::npos )
    configuration.insert( m_2015Menu.begin(),m_2015Menu.end() );
  if ( triggerMenu.find("2016")!=std::string::npos )
    configuration.insert( m_2016Menu.begin(),m_2016Menu.end() );

  if ( configuration.size() == 0 )
    {
      if ( m_2015Menu.find(triggerMenu) != m_2015Menu.end() )
	configuration[ triggerMenu ] = m_2015Menu.at(triggerMenu);
      else if ( m_2016Menu.find(triggerMenu) != m_2016Menu.end() )
	configuration[ triggerMenu ] = m_2016Menu.at(triggerMenu);
    }

  std::vector<std::string> output;
  for (std::map< std::string,std::string >::const_iterator it=configuration.begin(); it!=configuration.end(); it++)
    {
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

  // Check if chain is defined and return result
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

#ifndef ROOTCORE
  CHECK( evtStore()->retrieve(sgJetContainer,jetName) );
#else
  ANA_CHECK( evtStore()->retrieve(sgJetContainer,jetName) );
#endif

  if (sgJetContainer == 0) {
    msg(MSG::ERROR) << "Retrieved invalid Jet Input Container!" << endreq;
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
