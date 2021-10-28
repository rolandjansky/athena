/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfigSvc_HLTConfigSvc
#define TrigConfigSvc_HLTConfigSvc

#include <string>

#include "AthenaBaseComps/AthService.h"

namespace TrigConf {

  /**
   * @brief Service storing the HLT menu in the DetectorStore
   *
   */
  class HLTConfigSvc : public AthService {
  public:
    HLTConfigSvc(const std::string& name, ISvcLocator* pSvcLocator);

    virtual StatusCode initialize() override;

  private:
    StatusCode writeConfigToDetectorStore();

    Gaudi::Property<std::string> m_inputType{this, "InputType", "FILE",
      "FILE (json file), DB (Trigger DB)"};

    Gaudi::Property<std::string> m_hltFileName{this, "JsonFileName", "HLTMenu.json",
      "file name of HLT json file, needed if InputType is FILE"};

    Gaudi::Property<std::string> m_dbConnection{this, "TriggerDB", "TRIGGERDB",
      "DB connection alias, needed if InputType is DB"};

    Gaudi::Property<unsigned int> m_smk{this, "SMK", 0,
      "DB smk, needed if InputType is DB (optional for file InputType)"};
  };

} // namespace TrigConf

#endif
