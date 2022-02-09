/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfigSvc_LVL1ConfigSvc
#define TrigConfigSvc_LVL1ConfigSvc

#include <string>

#include "AthenaBaseComps/AthService.h"

namespace TrigConf {

  /**
   * Service storing the LVL1 menu in the DetectorStore.
   */
  class LVL1ConfigSvc : public AthService {
  public:
    LVL1ConfigSvc(const std::string& name, ISvcLocator* pSvcLocator);

    virtual StatusCode initialize() override;

  private:
    // loads Run-3-style menu (json) and writes it to the detector
    // store to be read by clients of the L1 configuration from there
    StatusCode loadRun3StyleMenu();

    Gaudi::Property<std::string> m_inputType{this, "InputType", "FILE",
      "FILE (json file), DB (Trigger DB)"};

    Gaudi::Property<std::string> m_l1FileName{this, "JsonFileName", "",
      "file name of L1 json file, needed if InputType is FILE"};


    Gaudi::Property<std::string> m_dbConnection{this, "TriggerDB", "TRIGGERDB",
      "DB connection alias, needed if InputType is DB"};

    Gaudi::Property<unsigned int> m_smk{this, "SMK", 0,
      "DB smk, needed if InputType is DB"};

  };

} // namespace TrigConf

#endif
