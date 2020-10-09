/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfig_DSConfigSvc
#define TrigConfig_DSConfigSvc

#include <stdint.h>

#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfHLTData/HLTFrame.h"

#include "GaudiKernel/ServiceHandle.h"
#include "./ConfigSvcBase.h"
#include "AthenaKernel/IIOVSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigConfInterfaces/ITrigConfigSvc.h"

#include <memory>
#include <set>

namespace TXC {
   class L1TopoMenu;
}

/**
 *    @short Service providing the full trigger configuration information, based
 *           on what is provided by HLTConfigSvc and LVL1ConfigSvc
 */
namespace TrigConf {

   class ThresholdConfig;
   class HLTChainList;
   class HLTSequenceList;
   class HLTPrescaleSet;
   class BunchGroup;

   /**
    * @brief Service providing the full trigger configuration information, based
    *        on what is provided by HLTConfigSvc and LVL1ConfigSvc
    *
    */
   class DSConfigSvc : public extends1<ConfigSvcBase, ITrigConfigSvc>
   {

   public:

      // Standard Gaudi Service constructor
      DSConfigSvc( const std::string& name, ISvcLocator* pSvcLocator );

      // Destructor
      virtual ~DSConfigSvc();

      // @brief initialize the service
      virtual StatusCode initialize() override;

      // @brief finalize the service
      virtual StatusCode finalize() override {
         return StatusCode::SUCCESS;
      }

      // @brief set the master key of the configuration to be requested
      virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIF ) override;

      // @brief L1 topo configuration menu
      virtual const TXC::L1TopoMenu* menu() const override {
         return m_topoMenu.get();
      }
      
      // LVL1 menu and prescales only (no bunchgroups, prescale clocks, etc.)
      virtual const CTPConfig* ctpConfig() const override {
         return & m_ctpConfig;
      }

      // access muctpi configuration
      virtual const Muctpi* muctpiConfig() const override {
         return & m_ctpConfig.muCTPi();
      }

      // access to trigger thresholds
      virtual const ThresholdConfig* thresholdConfig() const override {
         return & m_ctpConfig.menu().thresholdConfig();
      }

      // returns bunch group set
      virtual const BunchGroupSet* bunchGroupSet() const override {
         return & m_ctpConfig.bunchGroupSet();
      }

      // access to HLT chains
      virtual const HLTChainList* chainList() const override {
         return & m_hltFrame.getHLTChainList();
      }

      // access to HLT chains
      virtual const HLTChainList& chains() const override {
         return m_hltFrame.chains();
      }

      // access to HLT sequences
      virtual const HLTSequenceList* sequenceList() const override {
         return & m_hltFrame.getHLTSequenceList();
      }

      // access to HLT sequences
      virtual const HLTSequenceList& sequences() const override {
         return m_hltFrame.sequences();
      }

      // access to SMK
      virtual uint32_t masterKey() const override { return m_masterKey; }

      // access to LVL1 prescale key
      virtual uint32_t lvl1PrescaleKey() const override {
         return m_lvl1PsKey;
      }

      // access to HLT prescale configuration key
      virtual uint32_t hltPrescaleKey() const override { return m_hltPsKey; }

      // access to a description of the source of the configuration (TriggerDB/XMLfilename)
      virtual std::string configurationSource() const override {
         return m_configSrc;
      }

      virtual StatusCode updatePrescaleSets(uint requestcount) override;

      // This method is called by TrigSteer on *every* event (keep it fast)
      // This is never used in connection with COOL configuration data
      virtual StatusCode assignPrescalesToChains(uint lumiblock ) override;

   private:
      StatusCode registerCallbackForFolder( const std::string& foldername,
                                            bool multichannel );
      StatusCode update( IOVSVC_CALLBACK_ARGS_P( I, keys ) );
      StatusCode reset();
      void setEFLowerChainCounter();
      void set_ChainlistFromHltPrescaleSet();
      void set_HltPrescaleSetFromChainlist();

      StatusCode loadMenuFromDB( uint32_t smk, uint32_t l1psk, uint32_t hltpsk, uint32_t bgsk );

      std::unique_ptr<TXC::L1TopoMenu> m_topoMenu;

      CTPConfig            m_ctpConfig;       ///< LVL1 configuration
      HLTFrame             m_hltFrame;        ///< HLT configuration

      uint32_t             m_masterKey;       ///< configuration master key
      uint32_t             m_lvl1PsKey;       ///< lvl1 prescale key
      uint32_t             m_hltPsKey;        ///< hlt prescale key
      uint32_t             m_lvl1BgKey;       ///< lvl1 bunchgroup key
      std::string          m_configSrc;       ///< Description of the source of the configuration


      /// Handle to the Detector Store
      ServiceHandle< StoreGateSvc > m_detstore;

      ///to avoid crashes when a folder is not present and allow for lvl1/htl separation : 
      /// /* m_folders will take the info on folders from IOVDbSvc*/
      std::set<std::string> m_folders;


      // hasFolder(folder_name) will return true if folder_name is found in the string returned by IOVDbSvc, false otherwise
      bool hasFolder( const std::string& folder_name );

   }; // class DSConfigSvc

} // namespace TrigConf

#endif // TrigConfig_DSConfigSvc__H
