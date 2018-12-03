/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
//
//  ReadFromXmlDom.h
//  HDef
//
//  Created by sroe on 15/03/2016.
//

#ifndef IDVPM_ReadFromXmlDom_h
#define IDVPM_ReadFromXmlDom_h
#include "IReadHistoDef.h"
#include <vector>
#include "InDetPhysValMonitoring/SingleHistogramDefinition.h"
#include <xercesc/parsers/XercesDOMParser.hpp>



class ReadFromXmlDom: public IReadHistoDef {
public:
  ReadFromXmlDom();
  ReadFromXmlDom(const std::string& source);
  std::string source() const final;
  std::string format() const final;
  bool histoDefinitionMap(std::map<std::string, SingleHistogramDefinition>& usersMap) const final;
  bool insertDefinition(const SingleHistogramDefinition& oneDefinition);
  bool initialize() final;
  bool sourceExists() const final;
private:
  std::vector <SingleHistogramDefinition> m_vectorOfDefinitions;
  std::string m_source;
  const std::string m_format;
  SingleHistogramDefinition parseTextLine(const std::string& line);
  SingleHistogramDefinition parseTProfileText(const std::string& line);
  SingleHistogramDefinition parseXmlElement(const xercesc::DOMElement* element);
};
#endif /* ReadFromXmlDom_h */