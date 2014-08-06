// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigConfMetadataTool.h 603620 2014-06-25 17:55:37Z tamartin $
#ifndef TRIGGERD3PDMAKER_TRIGCONFMETADATATOOL_H
#define TRIGGERD3PDMAKER_TRIGCONFMETADATATOOL_H

// STL include(s):
#include <map>
#include <string>
#include <set>

// Gaudi/Athena include(s):
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"

// Trigger configuration include(s):
#include "TrigConfInterfaces/ITrigConfigSvc.h"

// D3PDMaker include(s):
#include "D3PDMakerInterfaces/IMetadataTool.h"
#include "D3PDMakerInterfaces/ID3PDSvc.h"
#include "TriggerD3PDMaker/TriggerKeyTool.h"

namespace D3PD {

   // Forward declaration(s):
   class ID3PD;

   /**
    *  @short Tool for dumping trigger configuration information into D3PDs
    *
    *         This tool can be used to save the trigger configuration information
    *         into D3PDs. It collects the information that may be needed to analyze
    *         the events that are processed during the job, and saves this information
    *         gradually during the job.
    *
    *         The tool implements two interfaces. It is a D3PD::IMetadataTool to
    *         be able to add it to the D3PD::MakerAlg as a metadata tool, and it's
    *         also an IIncidentListener. This latter interface allows it to listen
    *         to various "incidents" during the job. This is how the tool keeps
    *         track of which configurations were used during the job, and of which
    *         luminosity blocks are described by which configuration.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 603620 $
    * $Date: 2014-06-25 19:55:37 +0200 (Wed, 25 Jun 2014) $
    */
   class TrigConfMetadataTool : public AthAlgTool,
                                public IMetadataTool,
                                public IIncidentListener {

   public:
      /// Regular AlgTool constructor
      TrigConfMetadataTool( const std::string& type, const std::string& name,
                            const IInterface* parent );

      /// AlgTool initialization function
      virtual StatusCode initialize();
      /// Gaudi function describing which interfaces the tool implements
      virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );

      /// Function writing the configuration information to the D3PD
      virtual StatusCode writeMetadata( ID3PD* d3pd );

      /// Function called when a relevant incident happened
      virtual void handle( const Incident& inc );

   private:
      /// Function handling new trigger configurations
      void handleNewTrigConf( const Incident& inc );
      
      /// Function handling new trigger configurations using [TrigMonConfig] summary objects
      void handleNewTrigConfFromDataStore( const Incident& inc );

      /// Directory in which to store the configuration
      std::string m_configDir;

      /// Handle for the trigger configuration service
      ServiceHandle< TrigConf::ITrigConfigSvc > m_trigConfSvc;
      /// Property: TriggerKeyTool instance.
      ToolHandle<TriggerKeyTool> m_triggerKeyTool;
      /// Handle for the incident service
      ServiceHandle< IIncidentSvc > m_incidentSvc;
      /// Handle for the D3PDSvc:
      ServiceHandle< ID3PDSvc > m_d3pdSvc;
      /// Handle for the StoreGate service used by [TrigMonConfig]:
      ServiceHandle< StoreGateSvc > m_storeGate;  
      
      /// Storegate key for [TrigMonConfig] trigger configuration summary
      std::string m_keyConfig;
      
      /// Flag to use [TrigMonConfig] summaries instead of the default behaviour
      bool m_flagUseTrigConfEventSummaries;

      /// Cache of which trigger configurations have been "translated"
      std::set<TriggerKeyTool::Keys> m_configKeyCache;

      //
      // The configuration variables:
      //
      ID3PD* m_configTree; ///< D3PD object holding the configuration description
      int* m_smk; ///< Super Master Key
      int* m_l1psk; ///< LVL1 Prescale Key
      int* m_hltpsk; ///< HLT Prescale key
      std::map< std::string, int >*   m_lvl1ConfigNameMap; ///< LVL1 name -> CTP ID map
      std::map< std::string, float >* m_lvl1ConfigPSKMap; ///< LVL1 name -> prescale map
      std::map< std::string, int >*   m_hltConfigNameMap; ///< HLT name -> counter map
      std::map< std::string, float >* m_hltConfigPSKMap; ///< HLT name -> prescale map
      std::map< std::string, float >* m_hltConfigRPSKMap; ///< HLT name -> rerun prescale map
      std::map< std::string, float >* m_hltConfigPTMap; ///< HLT name -> passthrough map
      std::map< std::string, std::string >* m_hltConfigLowerChainNameMap; ///< HLT name -> seed name map
      
      //
      // The detailed configuration variables [TrigMonConfig]:  
      //     
      // variables for CHAIN
      uint16_t*                                  m_chainN;
      std::vector< uint32_t >*                   m_chainID;
      std::vector< uint16_t >*                   m_chainCounter;
      std::vector< std::string >*                m_chainName;
      std::vector< uint32_t >*                   m_chainLowerID;
      std::vector< std::vector< uint32_t > >*    m_chainLowerIDs;
      std::vector< std::string >*                m_chainLowerName;
      std::vector< uint16_t >*                   m_chainLowerCounter;
      std::vector< uint8_t >*                    m_chainLevel;
      std::vector< float >*                      m_chainPrescale;
      std::vector< float >*                      m_chainPassthrough;
      std::vector< std::vector< std::string > >* m_chainStreamNames;
      std::vector< std::vector< float > >*       m_chainStreamPrescales;
      std::vector< std::vector< std::string > >* m_chainGroupNames;
      std::vector< std::vector< std::string > >* m_chainEBHypoNames;
      // variables for CHAIN -> SIGNATURE
      std::vector< uint16_t >*                   m_chainSigN;
      std::vector< std::vector< uint32_t > >*    m_chainSigCounter;
      std::vector< std::vector< uint32_t > >*    m_chainSigLogic;
      std::vector< std::vector< std::string > >* m_chainSigLabel;
      std::vector< std::vector< uint16_t > >*    m_chainSigOutputTEn;
      std::vector< std::vector< uint32_t > >*    m_chainSigOutputTEIndex;
      std::vector< std::vector< uint32_t > >*    m_chainSigOutputTEs;
      // variables for SEQUENCE
      uint16_t* m_seqN;
      std::vector< uint32_t >*                   m_seqID;
      std::vector< uint16_t >*                   m_seqIndex;
      std::vector< std::string >*                m_seqName;
      std::vector< uint32_t >*                   m_seqTopoTEID;
      std::vector< std::vector< uint32_t > >*    m_seqInputTEs;
      // variables for SEQUENCE -> ALG
      std::vector< uint16_t >*                   m_seqAlgN;
      std::vector< std::vector< uint16_t > >*    m_seqAlgIndex;
      std::vector< std::vector< uint8_t > >*     m_seqAlgPosition;
      std::vector< std::vector< uint32_t > >*    m_seqAlgNameID;
      std::vector< std::vector< uint32_t > >*    m_seqAlgTypeID;
      std::vector< std::vector< std::string > >* m_seqAlgName;
      std::vector< std::vector< std::string > >* m_seqAlgTypeName;
      // variables for additional configuration data
      std::vector<std::string>*                  m_confVarKey;
      std::vector<uint32_t>*                     m_confVarVal;
      std::vector<std::string>*                  m_confStringKey;
      std::vector<std::string>*                  m_confStringVal;

   }; // class TrigConfMetadataTool

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_TRIGCONFMETADATATOOL_H
