/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerTranslatorSimple.h"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"

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
  for(const auto& item : m_trigmap_property) {
    ATH_MSG_DEBUG( "Key " << item.first << " Value " << item.second );
    std::vector<std::string> triggers;
    boost::split(triggers, item.second, boost::is_any_of(","));
    m_trigmap[item.first] = triggers;
  }
  return StatusCode::SUCCESS;
}


const std::vector<std::string> TriggerTranslatorToolSimple::translate(const std::string& key) const {
  return m_trigmap.at(key);
}
