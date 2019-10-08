/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfigSvc_LVL1ConfigSvc
#define TrigConfigSvc_LVL1ConfigSvc

#include <string>

#include "./ConfigSvcBase.h"

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

#include "TrigConfData/L1Menu.h"

class StoreGateSvc;

namespace TrigConf {

   /**
    *  $Date: 2009-02-04 09:12:03 $
    *   @short Service providing the LVL1 trigger configuration
    *
    *  @author $Author: stelzer $
    *  @version $Revision: 1.8 $
    *
    */
   class LVL1ConfigSvc : public extends1<ConfigSvcBase, ILVL1ConfigSvc> {
   public:

      LVL1ConfigSvc( const std::string& name, ISvcLocator* pSvcLocator );
      virtual ~LVL1ConfigSvc();
      
      StatusCode initialize();
      StatusCode finalize();

      StatusCode queryInterface( const InterfaceID& riid, void** ppvIF );

      // Access functions described by ILVL1ConfigSvc:
      const Muctpi* muctpiConfig() const { return m_muctpi; }
      const CTPConfig* ctpConfig() const { return m_ctpConfig; }
      const ThresholdConfig* thresholdConfig() const { return m_thrcfg; }
      const BunchGroupSet* bunchGroupSet() const;
      uint32_t lvl1PrescaleKey() const { return m_prescaleSetID; }

   private:

      StatusCode writeConfigToDetectorStore();

      // jobProperties
      Gaudi::Property< std::string > m_inputType { this, "InputType", "file", "file (json file), db (Trigger DB), cool (keys from cool, content from Trigger DB), none (no menu)" };
      Gaudi::Property< std::string > m_l1FileName { this, "JsonFileName", "L1Menu.json", "file name of L1 json file, needed if InputType is file" };
      Gaudi::Property< std::string > m_dbConnection { this, "TriggerDB", "TRIGGERDB", "DB connection alias, needed if InputType is db" };
      Gaudi::Property< unsigned int > m_smk { this, "SMK", 0, "DB smk, needed if InputType is db" };

      //SG::WriteHandleKey<TrigConf::L1Menu> m_l1MenuKey{"DetectorStore+L1TriggerMenu"};



      ::StoreGateSvc* m_detectorStore;
      
      // the configuration
      CTPConfig*       m_ctpConfig;
      ThresholdConfig* m_thrcfg;
      Muctpi*          m_muctpi;

      // Properties:
      //int m_masterID;
      int m_prescaleSetID;
      int m_bunchgroupSetID;
      BooleanProperty m_dumpTTVmap;

   };

}

#endif
