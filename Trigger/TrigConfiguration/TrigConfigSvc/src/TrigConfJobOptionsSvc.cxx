/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <nlohmann/json.hpp>

#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/Property.h"

#include "TrigConfJobOptionsSvc.h"

TrigConf::JobOptionsSvc::JobOptionsSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  base_class(name, pSvcLocator),
  m_josvc("JobOptionsSvc/TrigConfWrapped_JobOptionsSvc", name)
{}

StatusCode TrigConf::JobOptionsSvc::initialize()
{
  ATH_MSG_INFO("Initializing TrigConf::JobOptionsSvc");

  // Configure the wrapped JobOptionsSvc
  ATH_CHECK(m_josvc.retrieve());
  SmartIF<IProperty> joprop = &*m_josvc;
  ATH_CHECK(joprop->setProperty("TYPE", "NONE"));

  if (m_sourceType == "FILE") {
    // Read job options from JSON file
    ATH_CHECK(readOptions(m_sourcePath));
  }
  else if (m_sourceType == "DB") {
    // Here goes the code reading from the DB...
    ATH_CHECK(readOptions(m_sourcePath));
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

StatusCode TrigConf::JobOptionsSvc::start()
{
  // Dump job options to JSON file if configured (for debugging only!)
  if (!m_dump.empty()) {
    ATH_MSG_INFO("Writing job options to " << m_dump.value());
    dumpOptions(m_dump).ignore();
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigConf::JobOptionsSvc::readOptions(const std::string& file, const std::string& /*path*/)
{
  std::ifstream f(file);
  if (!f) {
    ATH_MSG_ERROR("Cannot open file " << file);
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Reading joboptions from " << file);
  nlohmann::json json;
  f >> json;

  for (const auto& [client, props] : json["properties"].items()) {
    for (const auto& [name, value] : props.items()) {
      ATH_CHECK(addPropertyToCatalogue(client, Gaudi::Property<std::string>(name, value.get<std::string>())));
    }
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
  for (const std::string& client : getClients()) {
    auto props = getProperties(client);
    if (props == nullptr) continue;
    json[client] = {};
    for (const Gaudi::Details::PropertyBase* p : *props) {
      if (p) json[client][p->name()] = p->toString();
    }
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
