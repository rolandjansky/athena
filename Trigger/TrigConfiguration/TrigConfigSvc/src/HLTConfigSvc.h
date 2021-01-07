/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfigSvc_HLTConfigSvc
#define TrigConfigSvc_HLTConfigSvc

#include "./ConfigSvcBase.h"

#include <string>
#include <vector>

#include "GaudiKernel/HistoProperty.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigConfInterfaces/IHLTConfigSvc.h"
#include "TrigConfHLTData/HLTFrame.h"

#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/HLTPrescalesSet.h"


class TH1F;
class TH2I;
class TrigTimer;

namespace TrigConf {

   class CTPConfig;
   class EventContext;
   class HLTChainList;
   class HLTSequenceList;
   class HLTMenu;
   class HLTPrescalesSet;
   class HLTPrescaleSet;
   class TrigDBConnectionConfig;

   /**
    * @brief Service providing the HLT trigger configuration chains and sequences
    *
    * This service should be accessed, to read the configuration lists
    */
   class HLTConfigSvc : public extends1<ConfigSvcBase, IHLTConfigSvc>
   {
   public:

      // implementing IIHLTConfigSvc
      const HLTChainList*    chainList() const __attribute__ ((deprecated));
      const HLTChainList&    chains() const;
      
      const HLTSequenceList* sequenceList() const __attribute__ ((deprecated));
      const HLTSequenceList& sequences() const;
      
      const HLTMenu& hltMenu(const EventContext&) const {
	const static TrigConf::HLTMenu dummy = TrigConf::HLTMenu();
	return dummy;
      }
      const HLTPrescalesSet& hltPrescalesSet(const EventContext&) const {
	const static TrigConf::HLTPrescalesSet dummy = TrigConf::HLTPrescalesSet();
	return dummy;
      }

      /*@brief constructor*/
      HLTConfigSvc( const std::string& name, ISvcLocator* pSvcLocator );
      virtual ~HLTConfigSvc();

      StatusCode initialize();
      StatusCode finalize();
      StatusCode start();

      StatusCode queryInterface( const InterfaceID& riid, void** ppvIF );

      StatusCode updatePrescaleSets(uint requestcount);

      // Access functions described by IHLTConfigSvc:
      const HLTFrame*        hltFrame() const { return &m_HLTFrame; }
      uint32_t               masterKey() const;
      uint32_t               hltPrescaleKey() const;

      void setL2LowerChainCounter(const CTPConfig*);

   private:

      StatusCode writeConfigToDetectorStore();

      // jobProperties
      Gaudi::Property< std::string > m_inputType { this, "InputType", "file", "file (json file), db (Trigger DB), cool (keys from cool, content from Trigger DB), none (no menu)" };
      Gaudi::Property< std::string > m_hltFileName { this, "JsonFileName", "HLTMenu.json", "file name of HLT json file, needed if InputType is file" };
      Gaudi::Property< std::string > m_dbConnection { this, "TriggerDB", "TRIGGERDB", "DB connection alias, needed if InputType is db" };
      Gaudi::Property< unsigned int > m_smk { this, "SMK", 0, "DB smk, needed if InputType is db" };

      StatusCode assignPrescalesToChains(uint lumiblock );

      StatusCode bookHistograms();
      void applyPrescaleSet(const HLTPrescaleSet& pss);
    
      enum ConfigSource { XML, ORACLE, MYSQL, SQLITE, DBLOOKUP };

      ServiceHandle<StoreGateSvc> m_eventStore;

      // The configuration:
      HLTFrame    m_HLTFrame; // what for is this varaible - seems unused ???

      uint        m_currentLumiblock { 0 };
      uint        m_currentPSS { 0 };

      // Properties:
      bool            m_setMergedHLT { true }; 
      bool            m_doMon { false };
   };
}

#endif
