/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef TRIGCONFIO__L1MENUCONDALG
#define TRIGCONFIO__L1MENUCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "TrigConfData/L1Menu.h"

namespace TrigConf {

   class L1MenuCondAlg : public AthReentrantAlgorithm {
   public:
      L1MenuCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
      virtual ~L1MenuCondAlg() = default;

      virtual StatusCode initialize() override;
      virtual StatusCode execute(const EventContext& ctx) const override;
      virtual StatusCode finalize() override;

   private:

      SG::WriteCondHandleKey<TrigConf::L1Menu> m_l1MenuOutputKey{ this, "L1Menu", "L1Menu", "L1 menu configuration"};

      StringProperty m_inputType { this, "InputType", "file", "file (json file), db (Trigger DB), cool (keys from cool, content from Trigger DB), none (no menu)" };
      StringProperty m_fileName { this, "JsonFileName", "LVL1config_Physics_pp_v7.json", "file name of json file, needed if InputType is file" };
      StringProperty m_dbConnection { this, "TriggerDB", "TRIGGERDB", "DB connection alias" };

   };

}
#endif
