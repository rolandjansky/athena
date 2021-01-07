/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfigSvc_LVL1ConfigSvc
#define TrigConfigSvc_LVL1ConfigSvc

#include <string>

#include "./ConfigSvcBase.h"

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

#include "TrigConfData/L1Menu.h"
#include "TrigConfData/L1PrescalesSet.h"
#include "TrigConfData/L1BunchGroupSet.h"


class StoreGateSvc;

namespace TrigConf {

   class EventContext;
   class L1Menu;
   class L1PrescalesSet;
   class L1BunchGroupSet;

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
      const ThresholdConfig* thresholdConfig() const;
      const BunchGroupSet* bunchGroupSet() const;
      uint32_t lvl1PrescaleKey() const { return static_cast<uint32_t>(m_prescaleSetID); }
      const L1Menu& l1Menu(const EventContext&) const {
	const static TrigConf::L1Menu dummy = TrigConf::L1Menu();
	return dummy;
      }
      const L1PrescalesSet& l1PrescalesSet(const EventContext&) const {
	const static TrigConf::L1PrescalesSet dummy = TrigConf::L1PrescalesSet();
	return dummy;
      }
      const L1BunchGroupSet& l1BunchGroupSet(const EventContext&) const {
	const static TrigConf::L1BunchGroupSet dummy = TrigConf::L1BunchGroupSet();
	return dummy;
      }

   private:

      // loads Run-2-style menu (xml) and keeps in memory to be
      // provided to clients of the L1 configuration via the access methods above
      StatusCode loadRun2StyleMenu();

      // loads Run-3-style menu (json) and writes it to the detector
      // store to be read by clients of the L1 configuration from there
      StatusCode loadRun3StyleMenu();

      /*
        Run 2-style menu (xml) initialization and loading

        This is still needed in the transition period and even
        afterwards still for reading Run 2 menu configuration. It

        * verifies and manipulates the job properties related to the Run 2 menu
        * calls loading Run 2 Style menu (xml) from either file or DB
      */
      StatusCode initializeRun2StyleMenu();

      /*
        Run 3-style menu (json) initialization and loading

        * verifies and manipulates the job properties related to the Run 3 menu
        * calls loading Run 3 Style menu (json) from either file or DB
      */
      StatusCode initializeRun3StyleMenu();


      // jobProperties for Run 3 style menu
      Gaudi::Property< std::string > m_inputType { this, "InputType", "file", "file (json file), db (Trigger DB), cool (keys from cool, content from Trigger DB), none (no menu)" };
      Gaudi::Property< std::string > m_l1FileName { this, "JsonFileName", "", "file name of L1 json file, needed if InputType is file" };
      Gaudi::Property< std::string > m_dbConnection { this, "TriggerDB", "TRIGGERDB", "DB connection alias, needed if InputType is db" };
      Gaudi::Property< unsigned int > m_smk { this, "SMK", 0, "DB smk, needed if InputType is db" };

      // Run 2 style configuration
      CTPConfig*       m_ctpConfig { nullptr };
      Muctpi*          m_muctpi { nullptr };

      // Properties:
      Gaudi::Property< int > m_prescaleSetID { this, "DBLVL1PSKey", 0, "L1 Prescale key"  };
      Gaudi::Property< int > m_bunchgroupSetID { this, "DBBGSKey", 0, "L1 Bunchgroup key"  };
      Gaudi::Property< bool > m_dumpTTVmap { this, "DumpTTVmap", false, "Dump threshold vector"  };
   };

}

#endif
