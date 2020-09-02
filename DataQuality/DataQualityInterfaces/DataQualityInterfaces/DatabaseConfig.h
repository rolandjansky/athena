/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef dqiDatabaseConfig_h
#define dqiDatabaseConfig_h

#include <iostream>
#include <optional>
#include <nlohmann/json.hpp>
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IDatabase.h"

namespace dqi {

class DatabaseConfig {

public:
  DatabaseConfig(std::string connectionString, long runNumber);
  nlohmann::json GetPayload(std::string tag);

  bool IsConnected() const;
  void Disconnect();

private:
  const std::string m_connectionString;
  const long m_runNumber;

  bool m_dbConnected;
  bool m_folderConnected;

  std::unordered_map<std::string, nlohmann::json> m_jsonData;
  cool::IFolderPtr m_folder;
  cool::IDatabasePtr m_database;

  long GetRunNumber() const;
  nlohmann::json LoadPayload(std::string tag);

  void Connect();
};

}
#endif
