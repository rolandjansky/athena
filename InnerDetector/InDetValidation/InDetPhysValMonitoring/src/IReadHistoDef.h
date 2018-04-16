/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  IReadHistoDef.h
//  HDef
//
//  Created by sroe on 08/07/2015.
//

#ifndef IReadHistoDef_h
#define IReadHistoDef_h
#include <map>
#include <string>
#include "InDetPhysValMonitoring/SingleHistogramDefinition.h"
class IReadHistoDef {
public:
  IReadHistoDef() {
  };
  virtual ~IReadHistoDef() {
  };
  virtual std::string source() const = 0;
  virtual std::string format() const = 0;
  virtual bool histoDefinitionMap(std::map<std::string, SingleHistogramDefinition>& usersMap) const = 0;
  virtual bool initialize() = 0;
  virtual bool sourceExists() const = 0;
private:
  virtual bool insertDefinition(const SingleHistogramDefinition& oneDefinition) = 0;
  std::map<std::string, SingleHistogramDefinition> m_histoDefinitionMap;
};

#endif
