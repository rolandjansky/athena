/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfig_DSConfigSvc
#define TrigConfig_DSConfigSvc

#include <stdint.h>

#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfHLTData/HLTFrame.h"

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/IIOVSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigConfInterfaces/ITrigConfigSvc.h"

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
   class DSConfigSvc : public AthService,
                       virtual public ITrigConfigSvc
   {

   public:
      /// Standard Gaudi Service constructor
      DSConfigSvc( const std::string& name, ISvcLocator* pSvcLocator );
      /// Destructor
      virtual ~DSConfigSvc();

      /**@brief initialize the service*/
      StatusCode initialize();

      /**@brief finalize the service*/
      StatusCode finalize();

      /**@brief set the master key of the configuration to be requested*/
      StatusCode queryInterface( const InterfaceID& riid, void** ppvIF );

      /**@brief L1 topo configuration menu*/
      const TXC::L1TopoMenu* menu() const;

      /// LVL1 menu and prescales only (no bunchgroups, prescale clocks, etc.)
      const CTPConfig* ctpConfig() const;

      // not available when reading from detector store
      // (source of config meta data is COOL or AOD-header)
      const Muctpi* muctpiConfig()             const;
      const ThresholdConfig* thresholdConfig() const;

      /**@brief BunchGroupSet
       *
       * @returns bunch group set
       */
      const BunchGroupSet* bunchGroupSet() const { return m_bunchGroupSet; }

      // HLT accessors
      const HLTChainList*    chainList() const;
      const HLTChainList&    chains() const { return m_hltFrame.chains(); }
      const HLTSequenceList* sequenceList() const;
      const HLTSequenceList& sequences() const { return m_hltFrame.sequences(); }

      /**@brief configuration master key
       *
       * @returns configuration master key
       */
      uint32_t masterKey() const { return m_masterKey; }

      /**@brief LVL1 prescale key
       *
       * @returns LVL1 prescale configuration key
       */
      uint32_t lvl1PrescaleKey() const { return m_lvl1PsKey; }

      /**@brief HLT prescale key
       *
       * @returns HLT prescale configuration key
       */
      uint32_t hltPrescaleKey() const { return m_hltPsKey; }

      /**@brief Configuration source
       *
       * @returns description of the source of the configuration (TriggerDB/XMLfilename)
       */
      const std::string& configurationSource() const { return m_configSrc; }

      StatusCode updatePrescaleSets(uint requestcount);

      StatusCode assignPrescalesToChains(uint lumiblock );

   private:
      StatusCode registerCallbackForFolder( const std::string& foldername,
                                            bool multichannel );
      StatusCode update( IOVSVC_CALLBACK_ARGS_P( I, keys ) );
      StatusCode reset();
      void setEFLowerChainCounter();
      void set_ChainlistFromHltPrescaleSet();
      void set_HltPrescaleSetFromChainlist();

      CTPConfig            m_ctpConfig;       ///< LVL1 configuration
      HLTFrame             m_hltFrame;        ///< HLT configuration

      ThresholdConfig    * m_thrcfg;          ///< list of thresholds
      HLTPrescaleSet     * m_prescaleSet;     ///< hlt prescales set
      uint32_t             m_masterKey;       ///< configuration master key
      uint32_t             m_lvl1PsKey;       ///< lvl1 prescale key
      uint32_t             m_hltPsKey;        ///< hlt prescale key
      uint32_t             m_lvl1BgKey;       ///< lvl1 bunchgroup key
      std::string          m_configSrc;       ///< Description of the source of the configuration


      /// Handle to the Detector Store
      ServiceHandle< StoreGateSvc > m_detstore;

      std::vector< unsigned char > m_itembgs;       ///< item assignments to bunchgroups
      TrigConf::BunchGroupSet*     m_bunchGroupSet; ///< bunchgroup definitions

      bool m_readyState;  ///< ready to deliver trigger configuration ?
      bool m_readLvl1Thr; ///< if set to true, read also the lvl1 thresholds
      bool m_readLvl1BG;  ///< if set to true, read also the lvl1 bunchgroups

      ///to avoid crashes when a folder is not present and allow for lvl1/htl separation : 
      /// /* std::string folders will take the info on folders from IOVDbSvc*/
      std::string m_folders;
      /// /* hasFolder(folder_name) will return true if folder_name is found in the string returned by IOVDbSvc, false otherwise */
      bool hasFolder( const std::string& folder_name );

   }; // class DSConfigSvc

} // namespace TrigConf

#endif // TrigConfig_DSConfigSvc__H
