/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/TriggerTranslatorSimple.h"

TriggerTranslatorToolSimple::TriggerTranslatorToolSimple(const std::string& type,
					     const std::string& name,
					     const IInterface* parent)
  : AthAlgTool( type, name, parent )
{
  declareInterface<ITriggerTranslatorTool>(this);
  declareProperty("triggerMapping", m_trigmap_property);
}

TriggerTranslatorToolSimple::~TriggerTranslatorToolSimple() {}

StatusCode TriggerTranslatorToolSimple::initialize() {
  std::vector<std::string> junk;
  //m_trigmap[""] = junk;
  for(const auto item : m_trigmap_property) {
    std::cout << "Key " << item.first << " Value " << item.second << std::endl;
  }
  return StatusCode::SUCCESS;
}


const std::vector<std::string> TriggerTranslatorToolSimple::translate(const std::string& key) {
  return m_trigmap[""];
}
