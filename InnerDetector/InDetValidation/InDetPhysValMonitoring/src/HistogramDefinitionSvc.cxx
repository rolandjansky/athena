/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
//  HistogramDefinitionSvc.cpp
//  HDef
//
//  Created by sroe on 07/07/2015.
//

#include "InDetPhysValMonitoring/HistogramDefinitionSvc.h"
#include "ReadFromXmlDom.h"
#include "IReadHistoDef.h"
namespace {
  const std::pair<float, float> invalidLimits = std::make_pair(
    std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());
  const std::string defaultFormat("text/plain");
  const std::string xmlFormat("text/xml");
}


HistogramDefinitionSvc::HistogramDefinitionSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator), m_format{UNKNOWN}, m_reader{} {
  declareProperty("DefinitionSource", m_source);
  declareProperty("DefinitionFormat", m_formatString = "text/plain");
}

HistogramDefinitionSvc::~HistogramDefinitionSvc() {
}

StatusCode
HistogramDefinitionSvc::initialize() {
  if (m_formatString.value() == defaultFormat) {
    m_format = TEXT_PLAIN;
  }
  if (m_formatString.value() == xmlFormat) {
    m_format = TEXT_XML;
  }
  if (m_format == UNKNOWN) {
    ATH_MSG_ERROR("Unknown format for the input to the hdef service");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("hdef source:" << m_source.value());
  ATH_MSG_INFO("Set format:" << m_formatString.value());
  ATH_MSG_INFO("format " << m_format);
  if (m_format == TEXT_PLAIN) {
    ATH_MSG_FATAL("Text format histogram definition files are deprecated.");
    return StatusCode::FAILURE;
  }
  if (m_format == TEXT_XML) {
    m_reader.reset(new ReadFromXmlDom(m_source.value()));
  }
  ATH_MSG_INFO("Reader initialising from " << m_reader->source() << " in " << m_reader->format() << " format.");
  bool ok = m_reader->initialize();
  if (not ok) {
    ATH_MSG_WARNING("Reader did not initialise");
  }
  ok = m_reader->histoDefinitionMap(m_histoDefMap);
  bool allDefsOk(true);
  for (auto& h:m_histoDefMap) {
    if (not h.second.isValid()) {
      ATH_MSG_WARNING("Invalid histogram definition: " << h.second.str());
      allDefsOk = false;
    }
  }
  if (ok and(not allDefsOk)) {
    ATH_MSG_WARNING("Some histogram definitions were bad.");
    return StatusCode::RECOVERABLE;
  }
  if (not ok) {
    ATH_MSG_ERROR("The definition reader failed to read the histogram definitions.");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode
HistogramDefinitionSvc::finalize() {
  ATH_MSG_INFO("Reader initialised from " << m_reader->source() << " in " << m_reader->format() << " format.");
  return StatusCode::SUCCESS;
}

SingleHistogramDefinition
HistogramDefinitionSvc::definition(const std::string& name, const std::string& dirName) const {
  SingleHistogramDefinition result;
  const auto pthisHistoPair(m_histoDefMap.find(SingleHistogramDefinition::stringIndex(name, dirName)));

  if (pthisHistoPair != m_histoDefMap.end()) {
    result = pthisHistoPair->second;
  }
  return result;
}

std::string
HistogramDefinitionSvc::histoType(const std::string& name, const std::string& dirName) const {
  std::string result {};
  const auto pthisHistoPair(m_histoDefMap.find(SingleHistogramDefinition::stringIndex(name, dirName)));

  if (pthisHistoPair != m_histoDefMap.end()) {
    result = pthisHistoPair->second.histoType;
  }
  return result;
}

std::string
HistogramDefinitionSvc::title(const std::string& name, const std::string& dirName) const {
  std::string result {};
  const auto pthisHistoPair(m_histoDefMap.find(SingleHistogramDefinition::stringIndex(name, dirName)));

  if (pthisHistoPair != m_histoDefMap.end()) {
    result = pthisHistoPair->second.title;
  }
  return result;
}

unsigned int
HistogramDefinitionSvc::nBinsX(const std::string& name, const std::string& dirName) const {
  unsigned int nbins(0);
  const auto pthisHistoPair(m_histoDefMap.find(SingleHistogramDefinition::stringIndex(name, dirName)));

  if (pthisHistoPair != m_histoDefMap.end()) {
    nbins = pthisHistoPair->second.nBinsX;
  }
  return nbins;
}

unsigned int
HistogramDefinitionSvc::nBinsY(const std::string& name, const std::string& dirName) const {
  unsigned int nbins(0);
  const auto pthisHistoPair(m_histoDefMap.find(SingleHistogramDefinition::stringIndex(name, dirName)));

  if (pthisHistoPair != m_histoDefMap.end()) {
    nbins = pthisHistoPair->second.nBinsY;
  }
  return nbins;
}

unsigned int
HistogramDefinitionSvc::nBinsZ(const std::string& name, const std::string& dirName) const {
  unsigned int nbins(0);
  const auto pthisHistoPair(m_histoDefMap.find(SingleHistogramDefinition::stringIndex(name, dirName)));

  if (pthisHistoPair != m_histoDefMap.end()) {
    nbins = pthisHistoPair->second.nBinsZ;
  }
  return nbins;
}

IHistogramDefinitionSvc::axesLimits_t
HistogramDefinitionSvc::xLimits(const std::string& name, const std::string& dirName) const {
  axesLimits_t result(invalidLimits);
  const auto pthisHistoPair(m_histoDefMap.find(SingleHistogramDefinition::stringIndex(name, dirName)));

  if (pthisHistoPair != m_histoDefMap.end()) {
    result = pthisHistoPair->second.xAxis;
  }
  return result;
}

IHistogramDefinitionSvc::axesLimits_t
HistogramDefinitionSvc::yLimits(const std::string& name, const std::string& dirName) const {
  axesLimits_t result(invalidLimits);
  const auto pthisHistoPair(m_histoDefMap.find(SingleHistogramDefinition::stringIndex(name, dirName)));

  if (pthisHistoPair != m_histoDefMap.end()) {
    result = pthisHistoPair->second.yAxis;
  }
  return result;
}

IHistogramDefinitionSvc::axesLimits_t
HistogramDefinitionSvc::zLimits(const std::string& name, const std::string& dirName) const {
  axesLimits_t result(invalidLimits);
  const auto pthisHistoPair(m_histoDefMap.find(SingleHistogramDefinition::stringIndex(name, dirName)));

  if (pthisHistoPair != m_histoDefMap.end()) {
    result = pthisHistoPair->second.zAxis;
  }
  return result;
}

std::string
HistogramDefinitionSvc::xTitle(const std::string& name, const std::string& dirName) const {
  std::string result {};
  const auto pthisHistoPair(m_histoDefMap.find(SingleHistogramDefinition::stringIndex(name, dirName)));

  if (pthisHistoPair != m_histoDefMap.end()) {
    result = pthisHistoPair->second.xTitle;
  }
  return result;
}

std::string
HistogramDefinitionSvc::yTitle(const std::string& name, const std::string& dirName) const {
  std::string result {};
  const auto pthisHistoPair(m_histoDefMap.find(SingleHistogramDefinition::stringIndex(name, dirName)));
  if (pthisHistoPair != m_histoDefMap.end()) {
    result = pthisHistoPair->second.yTitle;
  }
  return result;
}

std::string
HistogramDefinitionSvc::zTitle(const std::string& name, const std::string& dirName) const {
  std::string result {};
  const auto pthisHistoPair(m_histoDefMap.find(SingleHistogramDefinition::stringIndex(name, dirName)));
  if (pthisHistoPair != m_histoDefMap.end()) {
    result = pthisHistoPair->second.zTitle;
  }
  return result;
}

bool
HistogramDefinitionSvc::sourceExists() {
  return m_reader->sourceExists();
}

bool
HistogramDefinitionSvc::formatOk() {
  return (m_format < NFORMATS)and(m_format >= 0);
}

StatusCode
HistogramDefinitionSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (IIncidentListener::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<IIncidentListener*>(this);
  } else if (IHistogramDefinitionSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<IHistogramDefinitionSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}
