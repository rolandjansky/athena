/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfig_TrigConfigSvc
#define TrigConfig_TrigConfigSvc

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfInterfaces/IHLTConfigSvc.h"
#include "TrigConfL1Data/CTPConfig.h"

#include "TrigConfData/L1Menu.h"
#include "TrigConfData/L1PrescalesSet.h"
#include "TrigConfData/L1BunchGroupSet.h"
#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/HLTPrescalesSet.h"

#include "TrigConfHLTData/HLTFrame.h"

class EventContext;

#define DONOTUSE \
   ATH_MSG_ERROR("TrigConfigSvc::" <<__func__ << " is deprecated. Use xAODConfigSvc or L1/HLT menu from DetectorStore.")

namespace TrigConf {

   /**
    * Obsolete Run-2 wrapper service. Its only function currently is to initialize the
    * LVL1/HLTConfigSvc. All its interface methods are deprecated and the service should be
    * removed eventually.
    */
   class TrigConfigSvc : public extends<AthService, ITrigConfigSvc>
   {
   public:

      TrigConfigSvc( const std::string& name, ISvcLocator* pSvcLocator );

      virtual StatusCode initialize() override;

      // Lvl1 Accessors
      virtual const Muctpi*          muctpiConfig() const override { DONOTUSE; return nullptr; }
      virtual const CTPConfig*       ctpConfig() const override { DONOTUSE; return nullptr; }
      virtual const ThresholdConfig* thresholdConfig() const override { DONOTUSE; return nullptr; }
      virtual const BunchGroupSet*   bunchGroupSet() const override { DONOTUSE; return nullptr; }

      // HLT accessors
      virtual const HLTChainList&    chains() const override { DONOTUSE; return m_NullFrame.getHLTChainList(); }
      virtual const HLTSequenceList& sequences() const override{ DONOTUSE; return m_NullFrame.getHLTSequenceList(); }

      // Super Master Key - 0 if from XML, other cases TBI.
      virtual uint32_t masterKey() const override { DONOTUSE; return 0; }
      virtual uint32_t lvl1PrescaleKey() const override { DONOTUSE; return 0; }
      virtual uint32_t hltPrescaleKey() const override { DONOTUSE; return 0; }
      virtual std::string configurationSource() const override { DONOTUSE; return {}; }

      /// @name Dummy implementations of the Run 3 L1 JSON trigger configuration interface in IILVL1ConfigSvc.
      /// @brief Use the xAODConfigSvc or xAODConfigTool to access these data.
      /// @{
      virtual const ::TrigConf::L1Menu& l1Menu(const ::EventContext&) const override {
        DONOTUSE;
        const static ::TrigConf::L1Menu dummy = ::TrigConf::L1Menu();
        return dummy;
      }

      virtual const ::TrigConf::L1PrescalesSet& l1PrescalesSet(const ::EventContext&) const override {
        DONOTUSE;
        const static ::TrigConf::L1PrescalesSet dummy = ::TrigConf::L1PrescalesSet();
        return dummy;
      }

      virtual const ::TrigConf::L1BunchGroupSet& l1BunchGroupSet(const ::EventContext&) const override {
        DONOTUSE;
        const static ::TrigConf::L1BunchGroupSet dummy = ::TrigConf::L1BunchGroupSet();
        return dummy;
      }
      /// @}

      /// @name Dummy implementations of the Run 3 HLT JSON trigger configuration interface in IIHLTConfigSvc.
      /// @brief Use the xAODConfigSvc or xAODConfigTool to access these data.
      /// @{
      virtual const ::TrigConf::HLTMenu& hltMenu(const ::EventContext&) const override {
         DONOTUSE;
         const static ::TrigConf::HLTMenu dummy = ::TrigConf::HLTMenu();
         return dummy;
      }

      virtual const ::TrigConf::HLTPrescalesSet& hltPrescalesSet(const ::EventContext&) const override {
         DONOTUSE;
         const static ::TrigConf::HLTPrescalesSet dummy = ::TrigConf::HLTPrescalesSet();
         return dummy;
      }
      /// @}

   private:
      ServiceHandle<TrigConf::ILVL1ConfigSvc> m_lvl1Svc{this, "LVL1ConfigSvc",
           "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", "Provider for L1 menu in DetectorStore"};

      ServiceHandle<TrigConf::IHLTConfigSvc> m_hltSvc{this, "HLTConfigSvc",
           "TrigConf::HLTConfigSvc/HLTConfigSvc", "Provider for HLT menu in DetectorStore"};

      HLTFrame m_NullFrame;
   };

}

#endif
