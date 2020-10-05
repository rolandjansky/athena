/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <boost/algorithm/string.hpp>
#include <fstream>
#include <nlohmann/json.hpp>

#include "GaudiKernel/IProperty.h"
#include "Gaudi/Property.h"

#include "TrigConfIO/TrigDBJobOptionsLoader.h"
#include "TrigConfData/DataStructure.h"

#include "TrigConfJobOptionsSvc.h"

TrigConf::JobOptionsSvc::JobOptionsSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  base_class(name, pSvcLocator),
  m_josvc("JobOptionsSvc/TrigConfWrapped_JobOptionsSvc", name),
  m_optsvc("JobOptionsSvc/TrigConfWrapped_JobOptionsSvc", name)
{}

StatusCode TrigConf::JobOptionsSvc::initialize()
{
  ATH_MSG_INFO("Initializing TrigConf::JobOptionsSvc");

  // Configure the wrapped JobOptionsSvc
  ATH_CHECK(m_josvc.retrieve());
  SmartIF<IProperty> joprop = &*m_josvc;
  ATH_CHECK(joprop->setProperty("TYPE", "NONE"));

  ATH_CHECK(m_optsvc.retrieve());
  SmartIF<IService> josvc = &*m_josvc;
  m_optsvc->set( josvc->name() + ".TYPE" , "NONE" );

  if (m_sourceType == "FILE") {
    ATH_MSG_INFO("Reading joboptions from " << m_sourcePath.value());
    ATH_CHECK(readOptionsJson(m_sourcePath));
  }
  else if (m_sourceType == "DB") {
    parseDBString(m_sourcePath);
    ATH_MSG_INFO("Reading SMK " << m_smk << " from '" << m_server << "'");
    ATH_CHECK(readOptionsDB(m_server, m_smk));
  }
  else if (m_sourceType == "PYTHON") {
    /* "PYTHON" refers to loading properties directly from Python files
       which we do not use in ATLAS. Configuration via athena.py is of
       course supported by the default "NONE" mode. */
    ATH_MSG_ERROR("Not supported " << m_sourceType);
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

/**
 * Parse DB connection string and fill private members.
 * Format: `server=TRIGGERDB;smkey=42;lvl1key=43;hltkey=44`
 */
void TrigConf::JobOptionsSvc::parseDBString(const std::string& s)
{
  std::string key, val;
  std::istringstream iss(s);
  while (std::getline(std::getline(iss, key, '='), val, ';')) {
    boost::trim(key);
    boost::trim(val);
    if (key == "smkey")
      m_smk = std::stoi(val);
    else if (key == "server")
      m_server = val;
    else if (key == "lvl1key")
      m_l1psk = std::stoi(val);
    else if (key == "hltkey")
      m_hltpsk = std::stoi(val);
  }
}

StatusCode TrigConf::JobOptionsSvc::start()
{
  // Dump job options to JSON file if configured (for debugging only!)
  if (!m_dump.empty()) {
    ATH_MSG_INFO("Writing job options to " << m_dump.value());
    dumpOptions(m_dump).ignore();
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigConf::JobOptionsSvc::readOptionsJson(const std::string& file)
{
  std::ifstream f(file);
  if (!f) {
    ATH_MSG_ERROR("Cannot open file " << file);
    return StatusCode::FAILURE;
  }

  nlohmann::json json;
  f >> json;

  for (const auto& [client, props] : json["properties"].items()) {
    for (const auto& [name, value] : props.items()) {
      set(client + "." + name, value.get<std::string>());
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigConf::JobOptionsSvc::readOptionsDB(const std::string& db_server, int smk)
{
  // db job options loader
  TrigConf::TrigDBJobOptionsLoader jodbloader(db_server);

  TrigConf::DataStructure jo;
  jodbloader.loadJobOptions( smk, jo );
  if (jo) {
    unsigned int nClients(0), nProps(0);
    for( const auto & client : jo.getObject("properties").data()) {
      nClients++;
      for( const auto & property : client.second ) {
        nProps++;
        set(client.first + "." + property.first, property.second.data());
      }
    }
    ATH_MSG_INFO("Loaded job options from " << nClients << " clients with " << nProps << " in total");
  } else {
    ATH_MSG_FATAL("Could not load job options from database " << db_server << " with SMK " << smk);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


/**
 * This is mainly for debugging purposes and to compare the JobOptions
 * as seen by the JobOptionSvc to the ones extracted directly in Python.
 */
StatusCode TrigConf::JobOptionsSvc::dumpOptions(const std::string& file)
{
  // JSON filetype identifier
  nlohmann::json json_file;
  json_file["filetype"] = "joboptions";

  // Properties
  auto& json = json_file["properties"] = {};
  for (const auto& [name, value] : items()) {
    const size_t idot = name.rfind('.');
    const std::string client = name.substr(0, idot);
    const std::string propname = name.substr(idot+1);
    json[client][propname] = value;
  }

  // Write JSON to file
  std::ofstream o(file);
  if (!o) {
    ATH_MSG_ERROR("Cannot write to file " << file);
    return StatusCode::FAILURE;
  }
  o << std::setw(4) << json_file << std::endl;

  return StatusCode::SUCCESS;
}
