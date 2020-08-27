/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <nlohmann/json.hpp>

#include "CoolApplication/DatabaseSvcFactory.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IObject.h"

#include "DataQualityInterfaces/DatabaseConfig.h"
#include <array>

namespace dqi {

DatabaseConfig::
DatabaseConfig(std::string connectionString, long runNumber)
  : m_connectionString(connectionString)
  , m_runNumber((runNumber << 32) + 1)
  , m_dbConnected(false)
  , m_folderConnected(false)
{
}

void
DatabaseConfig::
Disconnect()
{
    if(m_dbConnected) {
      m_database->closeDatabase();
      m_dbConnected = false;
      m_folderConnected = false;
    }
}

void
DatabaseConfig::
Connect()
{
  try {
    cool::IDatabaseSvc& dbSvc = cool::DatabaseSvcFactory::databaseService();
    m_database = dbSvc.openDatabase(m_connectionString, true);

  } catch (std::exception& e) {
    std::cerr << "Unable to open database -> " << e.what() << '\n';
    return;
  }
  m_dbConnected = true;
  try {
    m_folder= m_database->getFolder("/GLOBAL/DQM/REFERENCES");

  } catch (std::exception& e) {
    std::cerr << "Unable loading folder for this tag/run-> " << e.what() << '\n';
    return;
  }
  m_folderConnected = true;
}

nlohmann::json
DatabaseConfig::
GetPayload(std::string tag)
{
  if(!m_dbConnected && !m_folderConnected) {
    Connect();
  }
  if(m_jsonData.find(tag) == m_jsonData.end()) {
    return LoadPayload(tag);
  }
  return m_jsonData.at(tag);
}

nlohmann::json
DatabaseConfig::
LoadPayload(std::string tag)
{
  nlohmann::json jsonData;
  if(m_dbConnected && m_folderConnected) {
    try {
      cool::IObjectPtr object = m_folder->findObject(GetRunNumber(), 1, tag);

      std::string data = object->payloadValue("jsonData");
      jsonData = nlohmann::json::parse(data);
      m_jsonData[tag] = jsonData;

    } catch (std::exception& e) {
      std::cerr << "Error: Failed loading payload for this tag/run-> " << e.what() << '\n';
    }
  }
  return jsonData;
}

bool
DatabaseConfig::
IsConnected() const
{
  return m_dbConnected && m_folderConnected;
}

long
DatabaseConfig::
GetRunNumber() const
{
  return m_runNumber;
}

}
