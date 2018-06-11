/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  ReadFromText.h
//  HDef
//
//  Created by sroe on 16/07/2015.
//

#ifndef ReadFromText_h
#define ReadFromText_h
#include "IReadHistoDef.h"
#include "InDetPhysValMonitoring/SingleHistogramDefinition.h"
#include <vector>

class ReadFromText: public IReadHistoDef {
public:
  ReadFromText();
  ReadFromText(const std::string& source);
  std::string source() const final;
  std::string format() const final;
  bool histoDefinitionMap(std::map<std::string, SingleHistogramDefinition>& usersMap) const;
  bool insertDefinition(const SingleHistogramDefinition& oneDefinition);
  bool initialize() final;
  bool sourceExists() const;
private:
  std::vector <SingleHistogramDefinition> m_vectorOfDefinitions;
  SingleHistogramDefinition parseTextLine(const std::string& line);
  std::string m_source;
  const std::string m_format;
};
#endif /* defined(ReadFromText_h) */
