/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  ReadFromXml.h
//  HDef
//
//  Created by sroe on 15/03/2016.
//

#ifndef IDVPM_ReadFromXml_h
#define IDVPM_ReadFromXml_h
#include "IReadHistoDef.h"
#include <vector>
#include "InDetPhysValMonitoring/SingleHistogramDefinition.h"


class ReadFromXml: public IReadHistoDef {
public:
  ReadFromXml();
  ReadFromXml(const std::string& source);
  std::string source() const final;
  std::string format() const final;
  bool histoDefinitionMap(std::map<std::string, SingleHistogramDefinition>& usersMap) const final;
  bool insertDefinition(const SingleHistogramDefinition& oneDefinition);
  bool initialize() final;
  bool sourceExists() const final;
private:
  std::vector <SingleHistogramDefinition> m_vectorOfDefinitions;
  SingleHistogramDefinition parseTextLine(const std::string& line);
  std::string m_source;
  const std::string m_format;
};
#endif /* ReadFromXml_h */