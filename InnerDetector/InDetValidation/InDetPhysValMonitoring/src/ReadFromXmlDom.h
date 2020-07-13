/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY; // Not sure if usage of Xerces-C++ here is thread safe.
// The following warning message is given if checked:
// warning: Use of static expression 'xercesc_3_1::XMLPlatformUtils::fgMemoryManager'
// of type 'xercesc_3_1::MemoryManager*' within function
// 'toNative(const XMLCh*)::<lambda(char*)>' may not be thread-safe.
//
// https://xerces.apache.org/xerces-c/faq-parse-3.html#faq-6
// Is Xerces-C++ thread-safe?
// The answer is yes if you observe the following rules for using Xerces-C++
// in a multi-threaded environment:
// ... ...

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
