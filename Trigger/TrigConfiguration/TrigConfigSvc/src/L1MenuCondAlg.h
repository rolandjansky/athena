/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef TRIGCONFIGSVC__L1MENUCONDALG
#define TRIGCONFIGSVC__L1MENUCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "TrigConfData/L1Menu.h"

namespace TrigConf {

   /**
    * @brief Condition algorithm to provide the L1 trigger menu
    *
    * The algorithm will provide the menu configuration through a conditions handle using \c TrigConf::L1Menu to present the data
    *
    * It is able to load the configuration in the following 3 ways
    * * File-based menu description: for most test environments: for test jobs with no database involved
    * * DB-based menu description where the DB keys are provided as algorithm job-properties: for reprocessing grid jobs with new menus
    * * DB-based menu description where the DB keys are taken from COOL: for offline reconstruction jobs
    */
   class L1MenuCondAlg : public AthReentrantAlgorithm {
   public:
      L1MenuCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
      virtual ~L1MenuCondAlg() = default;

      virtual StatusCode initialize() override;
      virtual StatusCode execute(const EventContext& ctx) const override;

   private:

      // conditions write handle for the L1 menu
      SG::WriteCondHandleKey<TrigConf::L1Menu> m_l1MenuOutputKey{ this, "L1Menu", "L1Menu", "L1 menu configuration"};

      // properties
      Gaudi::Property< std::string > m_inputType { this, "InputType", "file", "file (json file), db (Trigger DB), cool (keys from cool, content from Trigger DB), none (no menu)" };
      Gaudi::Property< std::string > m_fileName { this, "JsonFileName", "LVL1config_Physics_pp_v7.json", "file name of json file, needed if InputType is file" };
      Gaudi::Property< std::string > m_dbConnection { this, "TriggerDB", "TRIGGERDB", "DB connection alias" };

   };

}
#endif
