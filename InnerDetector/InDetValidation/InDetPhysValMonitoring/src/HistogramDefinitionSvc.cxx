/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  HistogramDefinitionSvc.cpp
//  HDef
//
//  Created by sroe on 07/07/2015.
//

#include "InDetPhysValMonitoring/HistogramDefinitionSvc.h"
#include "ReadFromText.h"
//#include "HardWiredDefinition.h"
#include "IReadHistoDef.h"
namespace{
  const std::pair<float, float> invalidLimits = std::make_pair(std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::quiet_NaN());
  const std::string defaultFormat("text/plain");
}


HistogramDefinitionSvc::HistogramDefinitionSvc(const std::string & name, ISvcLocator * pSvcLocator):
  AthService(name,pSvcLocator),m_format{},m_reader{}{
    declareProperty("DefinitionSource",m_source);
    declareProperty("DefinitionFormat",m_formatString=defaultFormat);
    
    if (m_formatString.value()== defaultFormat) m_format = TEXT_PLAIN;
    /**
       if (m_format==INCODE){
       m_reader.reset(new HardWiredDefinition());
       }
    **/
    
					  }

HistogramDefinitionSvc::~HistogramDefinitionSvc(){
}

StatusCode HistogramDefinitionSvc::initialize(){
  ATH_MSG_INFO("Initializing HistogramDefinitionSvc");
  ATH_MSG_INFO("File:"<<m_source.value());
  if (m_format==TEXT_PLAIN){
    m_reader.reset(new ReadFromText(m_source.value()));
  }
  ATH_MSG_INFO("Reported source: "<<m_reader->source());
  bool ok= m_reader->initialize();
  if (not ok) ATH_MSG_WARNING("Reader did not initialise");
  ok = m_reader->histoDefinitionMap(m_histoDefMap);
  if (ok) return StatusCode::SUCCESS;
  ATH_MSG_WARNING("Map not filled");
  return StatusCode::FAILURE;
}

StatusCode HistogramDefinitionSvc::finalize(){
    return StatusCode::SUCCESS;
}

SingleHistogramDefinition HistogramDefinitionSvc::definition(const std::string &name, const std::string & dirName) const{
  SingleHistogramDefinition result;
  const auto pthisHistoPair (m_histoDefMap.find(SingleHistogramDefinition::stringIndex(name,dirName)));
  if (pthisHistoPair != m_histoDefMap.end()){
    result=pthisHistoPair->second;
  }
  return result;
}

std::string HistogramDefinitionSvc::histoType(const std::string &name,const std::string &dirName) const{
  std::string result{};
  const auto pthisHistoPair (m_histoDefMap.find(SingleHistogramDefinition::stringIndex(name,dirName)));
  if (pthisHistoPair != m_histoDefMap.end()){
    result=pthisHistoPair->second.histoType;
  }
  return result;
}
std::string HistogramDefinitionSvc::title(const std::string &name,const std::string &dirName) const{
    std::string result{};
    const auto pthisHistoPair (m_histoDefMap.find(SingleHistogramDefinition::stringIndex(name,dirName)));
    if (pthisHistoPair != m_histoDefMap.end()){
      result=pthisHistoPair->second.title;
    }
    return result;
}
unsigned int HistogramDefinitionSvc::nBinsX(const std::string &name,const std::string &dirName) const{
  unsigned int nbins(0);
  const auto pthisHistoPair (m_histoDefMap.find(SingleHistogramDefinition::stringIndex(name,dirName)));
  if (pthisHistoPair != m_histoDefMap.end()){
    nbins=pthisHistoPair->second.nBinsX;
  }
  return nbins;
}

unsigned int HistogramDefinitionSvc::nBinsY(const std::string &name,const std::string &dirName) const{
  unsigned int nbins(0);
  const auto pthisHistoPair (m_histoDefMap.find(SingleHistogramDefinition::stringIndex(name,dirName)));
  if (pthisHistoPair != m_histoDefMap.end()){
    nbins=pthisHistoPair->second.nBinsY;
  }
  return nbins;
}

IHistogramDefinitionSvc::axesLimits_t HistogramDefinitionSvc::xLimits(const std::string &name,const std::string &dirName) const{
  axesLimits_t result(invalidLimits);
  const auto pthisHistoPair (m_histoDefMap.find(SingleHistogramDefinition::stringIndex(name,dirName)));
  if (pthisHistoPair != m_histoDefMap.end()){
    result=pthisHistoPair->second.xAxis;
  }
  return result;
}
IHistogramDefinitionSvc::axesLimits_t HistogramDefinitionSvc::yLimits(const std::string &name,const std::string &dirName) const {
  axesLimits_t result(invalidLimits);
  const auto pthisHistoPair (m_histoDefMap.find(SingleHistogramDefinition::stringIndex(name,dirName)));
  if (pthisHistoPair != m_histoDefMap.end()){
    result=pthisHistoPair->second.yAxis;
  }
  return result;
}

std::string HistogramDefinitionSvc::xTitle(const std::string &name,const std::string &dirName) const {
  std::string result{};
  const auto pthisHistoPair (m_histoDefMap.find(SingleHistogramDefinition::stringIndex(name,dirName)));
  if (pthisHistoPair != m_histoDefMap.end()){
    result=pthisHistoPair->second.xTitle;
  }
  return result;
}
std::string HistogramDefinitionSvc::yTitle(const std::string &name,const std::string &dirName) const {
  std::string result{};
  const auto pthisHistoPair (m_histoDefMap.find(SingleHistogramDefinition::stringIndex(name,dirName)));
  if (pthisHistoPair != m_histoDefMap.end()){
    result=pthisHistoPair->second.yTitle;
  }
  return result;
}


bool HistogramDefinitionSvc::sourceExists(){
  return m_reader->sourceExists();
}
bool HistogramDefinitionSvc::formatOk(){
  return (m_format<NFORMATS) and (m_format>=0);
}
StatusCode
HistogramDefinitionSvc::queryInterface(const InterfaceID & riid, void** ppvInterface ){
  if ( IIncidentListener::interfaceID().versionMatch(riid) ){
    *ppvInterface = dynamic_cast<IIncidentListener*>(this);
  } else if ( IHistogramDefinitionSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IHistogramDefinitionSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

