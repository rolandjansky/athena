/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"

// STL include(s):
#include "limits.h"

// Trigger configuration include(s):
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfL1Data/PrescaleSet.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTChain.h"

// Trigger [TrigMonConfig] data include(s):
#include "TrigMonitoringEvent/TrigMonConfigCollection.h"
#include "TrigMonitoringEvent/TrigMonEventCollection.h"
#include "TrigMonitoringEvent/TrigConfChain.h"
#include "TrigMonitoringEvent/TrigConfSeq.h"
#include "TrigMonitoringEvent/TrigConfSig.h"
#include "TrigMonitoringEvent/TrigConfAlg.h"

// D3PDMaker include(s):
#include "D3PDMakerInterfaces/ID3PD.h"

// Local include(s):
#include "TrigConfMetadataTool.h"

namespace D3PD {

   static const std::string TRIGCONF_INCIDENT_NAME = "TrigConf";

   TrigConfMetadataTool::TrigConfMetadataTool( const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent )
      : AthAlgTool( type, name, parent ),
        m_configDir( "" ),
        m_trigConfSvc( "TrigConf::TrigConfigSvc/TrigConfigSvc", name ),
        m_triggerKeyTool (),
        m_incidentSvc( "IncidentSvc", name ),
        m_d3pdSvc( "D3PD::RootD3PDSvc/TrigConfD3PDSvc", name ),
        m_storeGate( "StoreGateSvc", name),
        m_configKeyCache(),
        m_configTree( 0 ), m_smk( 0 ), m_l1psk( 0 ), m_hltpsk( 0 ),
        m_lvl1ConfigNameMap( 0 ), m_lvl1ConfigPSKMap( 0 ),
        m_hltConfigNameMap( 0 ), m_hltConfigPSKMap( 0 ),
        m_hltConfigRPSKMap( 0 ), m_hltConfigPTMap( 0 ),
        m_hltConfigLowerChainNameMap( 0 ),

        m_chainN (nullptr),
        m_chainID (nullptr),
        m_chainCounter (nullptr),
        m_chainName (nullptr),
        m_chainLowerID (nullptr),
        m_chainLowerIDs (nullptr),
        m_chainLowerName (nullptr),
        m_chainLowerCounter (nullptr),
        m_chainLevel (nullptr),
        m_chainPrescale (nullptr),
        m_chainPassthrough (nullptr),
        m_chainStreamNames (nullptr),
        m_chainStreamPrescales (nullptr),
        m_chainGroupNames (nullptr),
        m_chainEBHypoNames (nullptr),
        m_chainSigN (nullptr),
        m_chainSigCounter (nullptr),
        m_chainSigLogic (nullptr),
        m_chainSigLabel (nullptr),
        m_chainSigOutputTEn (nullptr),
        m_chainSigOutputTEIndex (nullptr),
        m_chainSigOutputTEs (nullptr),
        m_seqN (nullptr),
        m_seqID (nullptr),
        m_seqIndex (nullptr),
        m_seqName (nullptr),
        m_seqTopoTEID (nullptr),
        m_seqInputTEs (nullptr),
        m_seqAlgN (nullptr),
        m_seqAlgIndex (nullptr),
        m_seqAlgPosition (nullptr),
        m_seqAlgNameID (nullptr),
        m_seqAlgTypeID (nullptr),
        m_seqAlgName (nullptr),
        m_seqAlgTypeName (nullptr),
        m_confVarKey (nullptr),
        m_confVarVal (nullptr),
        m_confStringKey (nullptr),
        m_confStringVal (nullptr)
   {

      declareProperty( "ConfigDir", m_configDir = "/D3PD/d3pdMeta" );
      declareProperty( "TrigConfigSvc", m_trigConfSvc );
      declareProperty ("TriggerKeyTool", m_triggerKeyTool,
                       "TriggerKeyTool instance.");
      declareProperty( "IncidentSvc", m_incidentSvc );
      declareProperty( "D3PDSvc", m_d3pdSvc );
      declareProperty( "keyConfig", m_keyConfig = "HLT_OPI_HLT_monitoring_config"); ///< [TrigMonConfig] Name of TrigerConfigurationObject
      declareProperty( "UseTrigConfEventSummaries", m_flagUseTrigConfEventSummaries = true); ///< [TrigMonConfig] Use TrigMonConf objects
   }

   StatusCode TrigConfMetadataTool::initialize() {

      ATH_MSG_INFO( "Initializing. Package version: " << PACKAGE_VERSION );

      // Retrieve the needed services:
      CHECK( m_trigConfSvc.retrieve() );
      CHECK( m_incidentSvc.retrieve() );
      CHECK( m_d3pdSvc.retrieve() );
      CHECK( m_storeGate.retrieve() );

      // Set up the tool to listen to a few incidents:
      m_incidentSvc->addListener( this, TRIGCONF_INCIDENT_NAME );
      if( m_flagUseTrigConfEventSummaries == true ) {
        m_incidentSvc->addListener( this, IncidentType::EndEvent );
      }

      // Create the configuration objects through the D3PD service:
      CHECK( m_d3pdSvc->make( m_configDir + "/TrigConfTree", m_configTree ) );

      // Add the variables to the configuration "tree":
      CHECK( m_configTree->addVariable( "SMK",    m_smk,
                                        "Super Master Key" ) );
      CHECK( m_configTree->addVariable( "L1PSK",  m_l1psk,
                                        "LVL1 PreScale Key" ) );
      CHECK( m_configTree->addVariable( "HLTPSK", m_hltpsk,
                                        "HLT PreScale Key" ) );

      CHECK( m_configTree->addVariable( "LVL1NameMap", m_lvl1ConfigNameMap,
                                        "LVL1 item name -> CTP ID mapping" ) );
      CHECK( m_configTree->addVariable( "LVL1PrescaleMap", m_lvl1ConfigPSKMap,
                                        "LVL1 item name -> Prescale value mapping" ) );
      CHECK( m_configTree->addVariable( "HLTNameMap", m_hltConfigNameMap,
                                        "HLT chain name -> Chain ID mapping" ) );
      CHECK( m_configTree->addVariable( "HLTPrescaleMap", m_hltConfigPSKMap,
                                        "HLT chain name -> Prescale value mapping" ) );
      CHECK( m_configTree->addVariable( "HLTRerunPrescaleMap", m_hltConfigRPSKMap,
                                        "HLT chain name -> Re-run prescale value mapping" ) );
      CHECK( m_configTree->addVariable( "HLTPassthroughMap", m_hltConfigPTMap,
                                        "HLT chain name -> Passthrough value mapping" ) );
      CHECK( m_configTree->addVariable( "HLTLowerChainNameMap",
                                        m_hltConfigLowerChainNameMap,
                                        "HLT chain name -> Lower chain/item name mapping" ) );
                                        
      /// Begin [TrigMonConfig]
      if( m_flagUseTrigConfEventSummaries == true ) {   
         // variables for CHAIN
         CHECK( m_configTree->addVariable( "chainConfig_n", m_chainN, "Number of chains (including L1)" ) );
         CHECK( m_configTree->addVariable( "chainConfig_ID", m_chainID, "Hash value of chain name" ) );
         CHECK( m_configTree->addVariable( "chainConfig_counter", m_chainCounter, "HLT chain counter or L1 CTPID" ) );
         CHECK( m_configTree->addVariable( "chainConfig_name", m_chainName, "Chain name" ) );
         CHECK( m_configTree->addVariable( "chainConfig_lowerID", m_chainLowerID, "Hash of lower seeding chain name" ) );
         CHECK( m_configTree->addVariable( "chainConfig_lowerChainIDs", m_chainLowerIDs, "Vector of hashes of lower seeding chain names for multi-seeded chains" ) );
         CHECK( m_configTree->addVariable( "chainConfig_lowerName", m_chainLowerName, "Name of lower seeding chain(s), comma separated" ) );
         CHECK( m_configTree->addVariable( "chainConfig_lowerCounter", m_chainLowerCounter, "Counter of lower seeding chain" ) );
         CHECK( m_configTree->addVariable( "chainConfig_level", m_chainLevel, "Level of chain (1,2,3)" ) );
         CHECK( m_configTree->addVariable( "chainConfig_prescale", m_chainPrescale, "Chain prescale setting" ) );
         CHECK( m_configTree->addVariable( "chainConfig_passthrough", m_chainPassthrough, "Chain passthrough setting" ) );
         CHECK( m_configTree->addVariable( "chainConfig_streamNames", m_chainStreamNames, "Vector of chain stream names" ) );
         CHECK( m_configTree->addVariable( "chainConfig_streamPrescales", m_chainStreamPrescales, "Vector of stream prescales for chain" ) );
         CHECK( m_configTree->addVariable( "chainConfig_groupNames", m_chainGroupNames, "Vector of group names for chain" ) );
         CHECK( m_configTree->addVariable( "chainConfig_EBHypoNames", m_chainEBHypoNames, "Vector of L1 items from EB Hypo" ) );
         // variables for CHAIN -> SIGNATURE
         CHECK( m_configTree->addVariable( "chainConfig_sig_n", m_chainSigN, "Number of signatures in chain" ) );
         CHECK( m_configTree->addVariable( "chainConfig_sig_counter", m_chainSigCounter, "Signature position in chain" ) );
         CHECK( m_configTree->addVariable( "chainConfig_sig_logic", m_chainSigLogic, "Singmature logic" ) );
         CHECK( m_configTree->addVariable( "chainConfig_sig_label", m_chainSigLabel, "Singature label" ) );
         CHECK( m_configTree->addVariable( "chainConfig_sig_outputTE_n", m_chainSigOutputTEn, "Number of output Trigger Elements from signature" ) );
         CHECK( m_configTree->addVariable( "chainConfig_sig_outputTE_index", m_chainSigOutputTEIndex, "Index in vector containing Trigger Element outputs" ) );
         CHECK( m_configTree->addVariable( "chainConfig_sigOutputTEs", m_chainSigOutputTEs, "Vector of vector<TE_Outputs>, use outputTE_index to locate the correct entry" ) );
                                       
         // variables for SEQUENCE
         CHECK( m_configTree->addVariable( "seqConfig_n", m_seqN, "Number of sequences in configuration" ) );
         CHECK( m_configTree->addVariable( "seqConfig_outputTEID", m_seqID, "ID of trigger element output" ) );
         CHECK( m_configTree->addVariable( "seqConfig_outputTEIndex", m_seqIndex, "Index of output TE" ) );
         CHECK( m_configTree->addVariable( "seqConfig_outputTEName", m_seqName, "Name of output TE" ) );
         CHECK( m_configTree->addVariable( "seqConfig_topoTEID", m_seqTopoTEID, "ID of topo start TE" ) );
         CHECK( m_configTree->addVariable( "seqConfig_inputTEs", m_seqInputTEs, "Vector of input TE IDs" ) );
         // variables for SEQUENCE -> ALG
         CHECK( m_configTree->addVariable( "seqConfig_alg_n", m_seqAlgN, "Number of algorithms in sequence" ) );
         CHECK( m_configTree->addVariable( "seqConfig_alg_index", m_seqAlgIndex, "Unique algorithm index in current configuration" ) );
         CHECK( m_configTree->addVariable( "seqConfig_alg_position", m_seqAlgPosition, "Position of algorithm in sequence" ) );
         CHECK( m_configTree->addVariable( "seqConfig_alg_nameID", m_seqAlgNameID, "Hash of alg name" ) );
         CHECK( m_configTree->addVariable( "seqConfig_alg_typeID", m_seqAlgTypeID, "Hash of class name" ) );
         CHECK( m_configTree->addVariable( "seqConfig_alg_name", m_seqAlgName, "Algorithm instance name" ) );
         CHECK( m_configTree->addVariable( "seqConfig_alg_typeName", m_seqAlgTypeName, "Algorithm base class name" ) );
         // variables for misc other config data
         CHECK( m_configTree->addVariable( "config_miscVarKey", m_confVarKey, "Key to misc additional config int payload" ) );
         CHECK( m_configTree->addVariable( "config_miscVarVal", m_confVarVal, "Value of additional config int payload" ) );
         CHECK( m_configTree->addVariable( "config_miscStringKey", m_confStringKey, "Key to misc additional config string payload" ) );
         CHECK( m_configTree->addVariable( "config_miscStringVal", m_confStringVal, "alue of additional config string payload" ) );

      }
      /// End [TrigMonConfig]
                                 

      // Clear the cache, just to be sure:
      m_configKeyCache.clear();

      return StatusCode::SUCCESS;
   }

   StatusCode TrigConfMetadataTool::queryInterface( const InterfaceID& riid,
                                                    void** ppvIf ) {

      if( riid == IMetadataTool::interfaceID() ) {
         *ppvIf = static_cast< IMetadataTool* >( this );
         addRef();
         return StatusCode::SUCCESS;
      } else if( riid == IIncidentListener::interfaceID() ) {
         *ppvIf = static_cast< IIncidentListener* >( this );
         addRef();
         return StatusCode::SUCCESS;
      }

      return AthAlgTool::queryInterface( riid, ppvIf );
   }

   /**
    * The function is actually empty. The metadata should already be in the output
    * file at this point...
    */
   StatusCode TrigConfMetadataTool::writeMetadata( ID3PD* ) {

      ATH_MSG_INFO( "Trigger configuration available in the D3PD in directory: \""
                    << m_configDir << "\"" );

      return StatusCode::SUCCESS;
   }

   /**
    * This function is called whenever an incident happens that the tool
    * was set up to listen to. The function has to decide what kind of
    * incident it was, and the call the appropriate internal function to
    * handle that incident.
    *
    * @param inc The incident that happened
    */
   void TrigConfMetadataTool::handle( const Incident& inc ) {

      //
      // Handle trigger configuration change incidents:
      //
      if( inc.type() == TRIGCONF_INCIDENT_NAME ) {
         if( m_flagUseTrigConfEventSummaries == false ) {
            handleNewTrigConf( inc );
         }
         return;
      } else if( inc.type() == IncidentType::EndEvent ) { 
         if( m_flagUseTrigConfEventSummaries == true ) {
            handleNewTrigConfFromDataStore( inc );
         }
         return;
      }

      ATH_MSG_WARNING( "Received incident not recognised" );
      return;
   }
   
   
    /**
    * This function translates a *full* trigger configuration into a ROOT 
    * native format, instead of Oracle - it uses TrigConfEvent objects
    * from the trigger cost monitor as its source of summarised trigger
    * configuration data.
    *
    * To get this full summary, set UseTrigConfEventSummaries=True and
    * ensure that the cost monitoring infrastructure is enabled.
    */
   void TrigConfMetadataTool::handleNewTrigConfFromDataStore( const Incident& ) {
      
      const TrigMonConfigCollection *configCol = 0;
 
      if( !m_storeGate->contains<TrigMonConfigCollection>(m_keyConfig) ) {
         ATH_MSG_DEBUG( "No TrigMonConfigCollection in this event Key:" << m_keyConfig );
         return;
      } else {
         ATH_MSG_INFO( "TrigMonConfigCollection in this event Key:" << m_keyConfig << ". The configs will be saved." );
      }
   
      if( m_storeGate->retrieve(configCol, m_keyConfig).isFailure() || !configCol) {
         ATH_MSG_WARNING( "TrigMonConfigCollection present, however failed to retrieve from SG. Key:" << m_keyConfig );
         return;
      }
      
      ATH_MSG_DEBUG( "Got TrigMonConfigCollection: " << m_keyConfig << " Size:" << configCol->size() );

      for( TrigMonConfigCollection::const_iterator it = configCol->begin(); it != configCol->end(); ++it) {
         const TrigMonConfig* tmc = *it;
         if(!tmc) continue;

         TriggerKeyTool::Keys config_keys;

         // Find out if the configuration that's just been loaded by the
         // configuration service, has been already translated in this job.
         // In that case there's no reason to translate it again.
         // TimM - now use the info from the object
         config_keys.first = tmc->getMasterKey();
         config_keys.second.first = tmc->getLV1PrescaleKey();
         config_keys.second.second = tmc->getHLTPrescaleKey();
         if( m_configKeyCache.find( config_keys ) != m_configKeyCache.end() ) {
            ATH_MSG_INFO( "Configuration with keys from triggerKeyTool SMK: "
                           << config_keys.first << ", L1PSK: "
                           << config_keys.second.first
                           << ", HLT PSK: "
                           << config_keys.second.second
                         << " already translated" );
            continue;
         }

         // Let the user know what we're doing:
         // Use keys from tool as keys stored in object appear to be wrong
         // Object keys are tmc->getMasterKey(), tmc->getLV1PrescaleKey(), mc->getHLTPrescaleKey()
         ATH_MSG_INFO( "Translating trigger configuration from TrigMonConfig source" );
         ATH_MSG_INFO( "  SMK: " << config_keys.first
                    << ", L1 PSK: " << config_keys.second.first
                    << ", HLT PSK: " << config_keys.second.second ); 
  
         /// Save detailed Sequence and Algorithm details
         const std::vector<TrigConfSeq> configSeqs   = tmc->getVec<TrigConfSeq>();
         *m_seqN          = configSeqs.size();
         // variables for SEQUENCE
         m_seqID          ->clear();
         m_seqIndex       ->clear();
         m_seqName        ->clear();
         m_seqTopoTEID    ->clear();
         m_seqInputTEs    ->clear();
         //
         m_seqID          ->resize( configSeqs.size() );
         m_seqIndex       ->resize( configSeqs.size() );
         m_seqName        ->resize( configSeqs.size() );
         m_seqTopoTEID    ->resize( configSeqs.size() );
         m_seqInputTEs    ->resize( configSeqs.size() );
         // variables for SEQUENCE -> ALG
         m_seqAlgN        ->clear();
         m_seqAlgIndex    ->clear();
         m_seqAlgPosition ->clear();
         m_seqAlgNameID   ->clear();
         m_seqAlgTypeID   ->clear();
         m_seqAlgName     ->clear();
         m_seqAlgTypeName ->clear();
         //
         m_seqAlgN        ->resize( configSeqs.size() );
         m_seqAlgIndex    ->resize( configSeqs.size() );
         m_seqAlgPosition ->resize( configSeqs.size() );
         m_seqAlgNameID   ->resize( configSeqs.size() );
         m_seqAlgTypeID   ->resize( configSeqs.size() );
         m_seqAlgName     ->resize( configSeqs.size() );
         m_seqAlgTypeName ->resize( configSeqs.size() );
         for(unsigned int i = 0; i < configSeqs.size(); ++i) {
            m_seqID       ->at(i) = configSeqs.at(i).getId();
            m_seqIndex    ->at(i) = configSeqs.at(i).getIndex();
            m_seqName     ->at(i) = configSeqs.at(i).getName();
            m_seqTopoTEID ->at(i) = configSeqs.at(i).getTopoTE();
            m_seqInputTEs ->at(i) = configSeqs.at(i).getInputTEs(); //Direct fetch vector
            // Loop over Alg items
            const std::vector<TrigConfAlg> configSeqAlgs = configSeqs.at(i).getAlg();
            m_seqAlgN         ->at(i)  = configSeqAlgs.size();
            (m_seqAlgIndex    ->at(i)).resize( configSeqAlgs.size() );
            (m_seqAlgPosition ->at(i)).resize( configSeqAlgs.size() );
            (m_seqAlgNameID   ->at(i)).resize( configSeqAlgs.size() );
            (m_seqAlgTypeID   ->at(i)).resize( configSeqAlgs.size() );
            (m_seqAlgName     ->at(i)).resize( configSeqAlgs.size() );
            (m_seqAlgTypeName ->at(i)).resize( configSeqAlgs.size() );
            for (unsigned int j = 0; j < configSeqAlgs.size(); ++j) {
               (m_seqAlgIndex    ->at(i)).at(j) = configSeqAlgs.at(j).getIndex();    
               (m_seqAlgPosition ->at(i)).at(j) = configSeqAlgs.at(j).getPosition();
               (m_seqAlgNameID   ->at(i)).at(j) = configSeqAlgs.at(j).getNameId();
               (m_seqAlgTypeID   ->at(i)).at(j) = configSeqAlgs.at(j).getTypeId();
               (m_seqAlgName     ->at(i)).at(j) = configSeqAlgs.at(j).getName();
               (m_seqAlgTypeName ->at(i)).at(j) = configSeqAlgs.at(j).getType();
            }
         }
         
         /// Save Chain and Sig details
         int TEIndex = 0;
         const std::vector<TrigConfChain> configChains = tmc->getVec<TrigConfChain>();
         *m_chainN               = configChains.size();
         /// variables for CHAIN
         m_chainID               ->clear();
         m_chainCounter          ->clear();
         m_chainName             ->clear();
         m_chainLowerID          ->clear();
         m_chainLowerIDs         ->clear();
         m_chainLowerName        ->clear();
         m_chainLowerCounter     ->clear();
         m_chainLevel            ->clear();
         m_chainPrescale         ->clear();
         m_chainPassthrough      ->clear();
         m_chainStreamNames      ->clear();
         m_chainStreamPrescales  ->clear();
         m_chainGroupNames       ->clear();
         m_chainEBHypoNames      ->clear();
         //
         m_chainID               ->resize( configChains.size() );
         m_chainCounter          ->resize( configChains.size() );
         m_chainName             ->resize( configChains.size() );
         m_chainLowerID          ->resize( configChains.size() );
         m_chainLowerIDs         ->resize( configChains.size() );
         m_chainLowerName        ->resize( configChains.size() );
         m_chainLowerCounter     ->resize( configChains.size() );
         m_chainLevel            ->resize( configChains.size() );
         m_chainPrescale         ->resize( configChains.size() );
         m_chainPassthrough      ->resize( configChains.size() );
         m_chainStreamNames      ->resize( configChains.size() );
         m_chainStreamPrescales  ->resize( configChains.size() );
         m_chainGroupNames       ->resize( configChains.size() );
         m_chainEBHypoNames      ->resize( configChains.size() );
         /// variables for CHAIN -> SIGNATURE
         m_chainSigN             ->clear();
         m_chainSigCounter       ->clear();
         m_chainSigLogic         ->clear();
         m_chainSigLabel         ->clear();
         m_chainSigOutputTEn     ->clear();
         m_chainSigOutputTEIndex ->clear(); 
         m_chainSigOutputTEs     ->clear();
         //
         m_chainSigN             ->resize( configChains.size() );
         m_chainSigCounter       ->resize( configChains.size() );
         m_chainSigLogic         ->resize( configChains.size() );
         m_chainSigLabel         ->resize( configChains.size() );
         m_chainSigOutputTEn     ->resize( configChains.size() );
         m_chainSigOutputTEIndex ->resize( configChains.size() );
         // We cannot resize m_chainSigOutputTEs as we don't know a priori how big it'll be.
         for(unsigned int i = 0; i < configChains.size(); ++i) {
            m_chainID             ->at(i) = configChains.at(i).getId();
            m_chainCounter        ->at(i) = configChains.at(i).getCounter();
            m_chainName           ->at(i) = configChains.at(i).getChainName();
            m_chainLowerID        ->at(i) = configChains.at(i).getLowerId();
            m_chainLowerIDs       ->at(i) = configChains.at(i).getLowerIds(); //Direct vector fetch
            m_chainLowerName      ->at(i) = configChains.at(i).getLowerName();
            m_chainLowerCounter   ->at(i) = configChains.at(i).getLowerCounter();
            m_chainLevel          ->at(i) = configChains.at(i).getLevelId();
            m_chainPrescale       ->at(i) = configChains.at(i).getPrescale();
            m_chainPassthrough    ->at(i) = configChains.at(i).getPassThrough();
            m_chainStreamNames    ->at(i) = configChains.at(i).getStream();
            //m_chainStreamPrescales->at(i) = configChains.at(i).getStreamPS();
            m_chainGroupNames     ->at(i) = configChains.at(i).getGroup();
            m_chainEBHypoNames    ->at(i) = configChains.at(i).getEBHypo();
            // Loop over Sig items
            const std::vector<TrigConfSig> configChainSigs = configChains.at(i).getSignature();
            m_chainSigN->at(i)       = configChainSigs.size();
            (m_chainSigCounter       ->at(i)).resize( configChainSigs.size() );
            (m_chainSigLogic         ->at(i)).resize( configChainSigs.size() );
            (m_chainSigLabel         ->at(i)).resize( configChainSigs.size() );
            (m_chainSigOutputTEn     ->at(i)).resize( configChainSigs.size() );
            (m_chainSigOutputTEIndex ->at(i)).resize( configChainSigs.size() );
            for (unsigned int j = 0; j < configChainSigs.size(); ++j) {
               (m_chainSigCounter         ->at(i)).at(j) = configChainSigs.at(j).getCounter();
               (m_chainSigLogic           ->at(i)).at(j) = configChainSigs.at(j).getLogic();
               (m_chainSigLabel           ->at(i)).at(j) = configChainSigs.at(j).getLabel();
               // We shall not go deeper than vector<vector<int> >, so store this vector<int> "up one level"
               // and make a note of its position in the branch (TEIndex)
               (m_chainSigOutputTEn       ->at(i)).at(j) = configChainSigs.at(j).getOutputTEs().size();
               (m_chainSigOutputTEIndex   ->at(i)).at(j) = TEIndex++;
               m_chainSigOutputTEs        ->push_back( configChainSigs.at(j).getOutputTEs() ); //Direct vector fetch
            }      
         }

         // Save misc data
         m_confVarKey   ->clear();
         m_confVarVal   ->clear();
         m_confStringKey->clear();
         m_confStringVal->clear();
         m_confVarKey    ->assign( tmc->getVarName().begin(),tmc->getVarName().end() );
         m_confVarVal    ->assign( tmc->getVarId().begin()  ,tmc->getVarId().end()   );
         m_confStringKey ->assign( tmc->getPairKey().begin(),tmc->getPairKey().end() );
         m_confStringVal ->assign( tmc->getPairVal().begin(),tmc->getPairVal().end() );
 
         // Fill the information into the "classic" map format for compatability with other tools which utilise this data
         *m_smk    = config_keys.first;
         *m_l1psk  = config_keys.second.first;
         *m_hltpsk = config_keys.second.second;

         // Fill the LVL1 maps:
         ATH_MSG_DEBUG( "Filling config maps" );
         m_lvl1ConfigNameMap->clear();
         m_lvl1ConfigPSKMap->clear();
         // Fill the HLT maps:
         m_hltConfigNameMap->clear();
         m_hltConfigPSKMap->clear();
         m_hltConfigRPSKMap->clear();
         m_hltConfigPTMap->clear();
         m_hltConfigLowerChainNameMap->clear();
         for(unsigned int i = 0; i < configChains.size(); ++i) {
            if( configChains.at(i).getLevelId() == 1 ) {

               ( *m_lvl1ConfigNameMap )[ configChains.at(i).getChainName() ] = configChains.at(i).getCounter();
               ( *m_lvl1ConfigPSKMap )[ configChains.at(i).getChainName() ] = configChains.at(i).getPrescale();

                REPORT_MESSAGE( MSG::VERBOSE )
                   << "   \"" << configChains.at(i).getChainName()
                   << "\" CTP id = " << configChains.at(i).getCounter()
                   << ", prescale = "
                   << configChains.at(i).getPrescale();
            } else {

               // "classic" tool maps
               ( *m_hltConfigNameMap )[ configChains.at(i).getChainName() ]           = configChains.at(i).getCounter();
               ( *m_hltConfigPSKMap )[ configChains.at(i).getChainName() ]            = configChains.at(i).getPrescale();
               ( *m_hltConfigRPSKMap )[ configChains.at(i).getChainName() ]           = 0; // This information is not present in the summary
               ( *m_hltConfigPTMap )[ configChains.at(i).getChainName() ]             = configChains.at(i).getPassThrough();
               ( *m_hltConfigLowerChainNameMap )[ configChains.at(i).getChainName() ] = configChains.at(i).getLowerName();

               REPORT_MESSAGE( MSG::VERBOSE )
                  << "   \"" << configChains.at(i).getChainName()
                  << "\" Chain counter = "
                  << configChains.at(i).getCounter()
                  << ", prescale = "
                  << configChains.at(i).getPrescale();
            }
         }
       
         // Fill the configuration into the output:
         if( m_configTree->capture().isFailure() ) {
            ATH_MSG_ERROR( "Couldn't save the configuration." );
            return;
         } else {
            ATH_MSG_INFO( "Trigger configuration from TrigMonConfig source successfully saved." );
         }

         // Update the book-keeping variable:
         m_configKeyCache.insert( config_keys );
      }   
   }
   
   /**
    * This function is responsible for translating each new configuration
    * into a ROOT native format. It tries to be smart enough to only
    * "translate" each configuration once.
    */
   void TrigConfMetadataTool::handleNewTrigConf( const Incident& ) {

      //
      // Find out if the configuration that's just been loaded by the
      // configuration service, has been already translated in this job.
      // In that case there's no reason to translate it again.
      //
      // (For instance the trigger could switch there and back between
      // different prescale keys within a run.)
      //
      TriggerKeyTool::Keys config_keys = m_triggerKeyTool->getKeys();
      if( m_configKeyCache.find( config_keys ) != m_configKeyCache.end() ) {
         ATH_MSG_DEBUG( "Configuration with keys SMK: "
                        << config_keys.first << ", L1PSK: "
                        << config_keys.second.first
                        << ", HLT PSK: "
                        << config_keys.second.second
                        << " already translated" );
         return;
      }

      // Let the user know what we're doing:
      ATH_MSG_INFO( "Translating trigger configuration" );
      ATH_MSG_INFO( "  SMK: " << config_keys.first
                    << ", L1 PSK: " << config_keys.second.first
                    << ", HLT PSK: " << config_keys.second.second );

      // Fill the configuration keys:
      *m_smk    = config_keys.first;
      *m_l1psk  = config_keys.second.first;
      *m_hltpsk = config_keys.second.second;

      // Fill the LVL1 maps:
      ATH_MSG_DEBUG( "Filling LVL1 maps" );
      m_lvl1ConfigNameMap->clear();
      m_lvl1ConfigPSKMap->clear();
      TrigConf::ItemContainer::const_iterator item_itr =
         m_trigConfSvc->ctpConfig()->menu().items().begin();
      TrigConf::ItemContainer::const_iterator item_end =
         m_trigConfSvc->ctpConfig()->menu().items().end();
      std::vector< float > prescales =
         m_trigConfSvc->ctpConfig()->prescaleSet().prescales_float();
      for( ; item_itr != item_end; ++item_itr ) {
         ( *m_lvl1ConfigNameMap )[ ( *item_itr )->name() ] = ( *item_itr )->ctpId();
         ( *m_lvl1ConfigPSKMap )[ ( *item_itr )->name() ] =
            prescales[ ( *item_itr )->ctpId() ];

         if( msgLvl( MSG::VERBOSE ) ) {
            REPORT_MESSAGE( MSG::VERBOSE )
               << "   \"" << ( *item_itr )->name()
               << "\" CTP id = " << ( *item_itr )->ctpId()
               << ", prescale = "
               << prescales[ ( *item_itr )->ctpId() ];
         }
      }

      // Fill the HLT maps:
      ATH_MSG_DEBUG( "Filling HLT maps" );
      m_hltConfigNameMap->clear();
      m_hltConfigPSKMap->clear();
      m_hltConfigRPSKMap->clear();
      m_hltConfigPTMap->clear();
      m_hltConfigLowerChainNameMap->clear();

      for( const TrigConf::HLTChain* chain : m_trigConfSvc->chains() ) {

         ( *m_hltConfigNameMap )[ chain->chain_name() ] =
            chain->chain_counter();
         ( *m_hltConfigPSKMap )[ chain->chain_name() ] =
            chain->prescale();
         ( *m_hltConfigRPSKMap )[ chain->chain_name() ] =
           chain->prescales().getRerunPrescale("").second;
         ( *m_hltConfigPTMap )[ chain->chain_name() ] =
            chain->pass_through();
         ( *m_hltConfigLowerChainNameMap )[ chain->chain_name() ] =
            chain->lower_chain_name();

         if( msgLvl( MSG::VERBOSE ) ) {
            REPORT_MESSAGE( MSG::VERBOSE )
               << "   \"" << chain->chain_name()
               << "\" Chain counter = "
               << chain->chain_counter()
               << ", prescale = "
               << chain->prescale();
         }
      }

      // Fill the configuration into the output:
      if( m_configTree->capture().isFailure() ) {
         REPORT_MESSAGE( MSG::ERROR )
            << "Couldn't save the configuration for "
            << "  SMK: " << config_keys.first
            << ", L1 PSK: " << config_keys.second.first
            << ", HLT PSK: " << config_keys.second.second;
         return;
      }

      // Update the book-keeping variable:
      m_configKeyCache.insert( config_keys );

      return;
   }

} // namespace D3PD
