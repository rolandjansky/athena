/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfigSvc_LVL1ConfigSvc
#define TrigConfigSvc_LVL1ConfigSvc

#include <string>

#include "TrigConfigSvc/ConfigSvcBase.h"

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

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
