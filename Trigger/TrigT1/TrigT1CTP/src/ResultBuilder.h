/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT1CTP_RESULTBUILDER_H
#define TRIGT1CTP_RESULTBUILDER_H

// local includes
//#include "./SpecialTrigger.h"
#include "./ThresholdMap.h"
#include "./ItemMap.h"

#include <stdint.h>

// For handling different CTP versions:
#include "CTPfragment/CTPdataformatVersion.h"
#include "GaudiKernel/EventIDBase.h"

#include "AthenaBaseComps/AthAlgTool.h"

// L1 configuration data
#include "TrigConfData/L1Menu.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerItem.h"

// Output to the RoIB:
#include "TrigT1Interfaces/CTPSLink.h"

// Output to the DAQ:
#include "TrigT1Result/CTP_RDO.h"

#include "CLHEP/Random/RandomEngine.h"

namespace LVL1CTP {

   /**
    *
    *   @short Class used to create the CTP output (RDO, SLink and TriggerInfo)
    */
   
   class ResultBuilder : public AthAlgTool {

   public:
      
      ResultBuilder( const std::string& type, const std::string& name, const IInterface* parent );
      ~ResultBuilder();

      void setRandomEngine( CLHEP::HepRandomEngine* rndmEngine );

      StatusCode setConfiguration( const TrigConf::CTPConfig* ctpConfig, 
                                   const TrigConf::L1Menu* l1menu ) const;

      StatusCode buildItemDecision( const std::map<std::string, unsigned int> & thrMultiMap,
                                    std::map<std::string, unsigned int> & itemDecisionMap,
                                    CLHEP::HepRandomEngine* rndmEngine ) const;
      

      StatusCode constructResultVectors( const std::map<std::string, unsigned int> & itemDecisionMap,
                                         std::vector<uint32_t> & tbp, std::vector<uint32_t> & tap, std::vector<uint32_t> & tav,
                                         unsigned char & triggerType ) const;


      StatusCode constructTIPVector( const std::map<std::string, unsigned int> & thrMultiMap,
                                     std::vector<uint32_t> & tip ) const;


      //! build RoI result (LVL1CTP::CTPSLink)
      std::unique_ptr<CTPSLink> constructRoIResult( const EventIDBase & eventID, 
                                                    const std::vector<uint32_t> & tbp, const std::vector<uint32_t> & tap, 
                                                    const std::vector<uint32_t> & tav, const std::vector<uint32_t> & tip, 
                                                    const unsigned char triggerType ) const;

      //! build RDO result (CTP_RDO)
      std::unique_ptr<CTP_RDO> constructRDOResult( const EventIDBase & eventID, 
                                                   const std::vector<uint32_t> & tbp, const std::vector<uint32_t> & tap, 
                                                   const std::vector<uint32_t> & tav, const std::vector<uint32_t> & tip ) const;

   private:

      struct ConfigSource {
      ConfigSource(const TrigConf::CTPConfig* ctpConfig, const TrigConf::L1Menu* l1menu) : m_ctpConfig(ctpConfig), m_l1menu(l1menu) {}
         const TrigConf::CTPConfig* m_ctpConfig {nullptr}; // run 2
         const TrigConf::L1Menu * m_l1menu { nullptr }; // run 3
         const TrigConf::CTPConfig* ctpConfig() const { return m_ctpConfig; }
         const TrigConf::L1Menu* l1menu() const { return m_l1menu; }
      };

      enum WrdType { TBP = 0x01, TAP = 0x02, TAV = 0x04 };

      StatusCode createTriggerConfigMaps(const ConfigSource & cfgSrc) const;

      //! build list of fired items and dump to string
      std::vector<std::string> firedItems(const std::vector<uint32_t>& triggerWords) const;

      // configuration information
      mutable std::unique_ptr<ThresholdMap>         m_thrConfigMap ATLAS_THREAD_SAFE { nullptr };    //!< Map between threshold objects and their CTP-internal description
      mutable std::unique_ptr<ItemMap>              m_itemConfigMap ATLAS_THREAD_SAFE { nullptr };   //!< Map between item objects and their CTP-internal description
      mutable InternalTriggerMap    m_internalTrigger ATLAS_THREAD_SAFE;             //!< internal triggers BGRP and RNDM
      unsigned int                  m_ctpVersionNumber { 4 };      //!< CTP data format version (4 in most of Run 2 and in Run 3) 
      CTPdataformatVersion*         m_ctpDataFormat { nullptr };   //!< CTP data format details
   };


   // typedefs

   // generic typedef for easy access to TIP/TBP/TAP/TAV information
   typedef std::vector<uint32_t> (ResultBuilder::*Result)() const;

} // namespace LVL1CTP

#endif // TRIGT1CTP_RESULTBUILDER_H
