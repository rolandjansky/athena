/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfigSvc_HLTConfigSvc
#define TrigConfigSvc_HLTConfigSvc

#include "ConfigSvcBase.h"

#include "CxxUtils/checker_macros.h"

#include "TrigConfInterfaces/IHLTConfigSvc.h"
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"
#include "TrigConfHLTData/HLTSequenceList.h"
#include "TrigConfHLTData/HLTChainList.h"

#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/HLTPrescalesSet.h"

#include <string>

class EventContext;

namespace TrigConf {

   class CTPConfig;

   /**
    * @brief Service providing the HLT trigger configuration chains and sequences
    *
    * This service should be accessed, to read the configuration lists
    */
   class HLTConfigSvc : public extends<ConfigSvcBase, IHLTConfigSvc>
   {
   public:

      // implementing IIHLTConfigSvc
      virtual const HLTChainList&    chains() const override { return m_HLTFrame.getHLTChainList(); }
      virtual const HLTSequenceList& sequences() const override { return m_HLTFrame.getHLTSequenceList(); }

      /// @name Dummy implementations of the Run 3 HLT JSON trigger configuration interface in IIHLTConfigSvc.
      /// @brief Use the xAODConfigSvc or xAODConfigTool to access these data.
      /// @{
      virtual const ::TrigConf::HLTMenu& hltMenu(const ::EventContext&) const override {
         const static ::TrigConf::HLTMenu dummy = ::TrigConf::HLTMenu();
         return dummy;
      }

      virtual const ::TrigConf::HLTPrescalesSet& hltPrescalesSet(const ::EventContext&) const override {
         const static ::TrigConf::HLTPrescalesSet dummy = ::TrigConf::HLTPrescalesSet();
         return dummy;
      }
      /// @}

      /*@brief constructor*/
      HLTConfigSvc( const std::string& name, ISvcLocator* pSvcLocator );

      virtual StatusCode initialize ATLAS_NOT_THREAD_SAFE () override;
      virtual StatusCode start() override;

      // Access functions described by IHLTConfigSvc:
      void setL2LowerChainCounter(const CTPConfig*);
      const HLTFrame*   hltFrame() const { return &m_HLTFrame; }
      virtual uint32_t  masterKey() const override { return m_HLTFrame.smk(); }
      virtual uint32_t  hltPrescaleKey() const override {
         return m_HLTFrame.getPrescaleSet() ? m_HLTFrame.getPrescaleSet()->id() : 0;
      }

   private:

      StatusCode writeConfigToDetectorStore();

      // jobProperties
      Gaudi::Property< std::string > m_inputType { this, "InputType", "file", "file (json file), db (Trigger DB), cool (keys from cool, content from Trigger DB), none (no menu)" };
      Gaudi::Property< std::string > m_hltFileName { this, "JsonFileName", "HLTMenu.json", "file name of HLT json file, needed if InputType is file" };
      Gaudi::Property< std::string > m_dbConnection { this, "TriggerDB", "TRIGGERDB", "DB connection alias, needed if InputType is db" };
      Gaudi::Property< unsigned int > m_smk { this, "SMK", 0, "DB smk, needed if InputType is db (optional for file InputType)" };

      enum ConfigSource { XML, ORACLE, MYSQL, SQLITE, DBLOOKUP };

      // The configuration:
      HLTFrame    m_HLTFrame;

      // Properties:
      bool            m_setMergedHLT { true }; 
   };
}

#endif
