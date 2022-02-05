/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfigSvc_HLTConfigSvc
#define TrigConfigSvc_HLTConfigSvc

#include <string>

#include "AthenaBaseComps/AthService.h"

namespace TrigConf {

  /**
   * Service storing the HLT menu in the DetectorStore.
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

    Gaudi::Property<std::string> m_monitoringFileName{this, "MonitoringJsonFileName", "",
      "file name of HLT monitoring json file, optional if InputType is FILE"};

    Gaudi::Property<std::string> m_dbConnection{this, "TriggerDB", "TRIGGERDB",
      "DB connection alias, needed if InputType is DB"};

    Gaudi::Property<bool> m_monitoringOptional{this, "MonitoringJSONOptional", true,
      "Flag to control if a JSON is required when MonitoringJsonFileName is supplied and InputType is FILE"};

    Gaudi::Property<unsigned int> m_smk{this, "SMK", 0,
      "DB smk, needed if InputType is DB (optional for file InputType)"};
  };

} // namespace TrigConf

#endif
