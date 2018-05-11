/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  ReadFromXml.cxx
//  HDef
//
//  Created by sroe on 15/03/2016.
//

#include "ReadFromXml.h"
#include "xmlUtilities.h"
#include "XmlContentHandlers.h"
#include "ErrorHandler.h"
#include <sys/stat.h>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include "PathResolver/PathResolver.h"

ReadFromXml::ReadFromXml() : m_source("unspecified file"), m_format("text/xml") {
}

ReadFromXml::ReadFromXml(const std::string& source) : m_format("text/xml") {
  m_source = PathResolver::find_file(source, "DATAPATH");
}

std::string
ReadFromXml::source() const {
  return m_source;
}

std::string
ReadFromXml::format() const {
  return m_format;
}

bool
ReadFromXml::histoDefinitionMap(std::map<std::string, SingleHistogramDefinition>& usersmap) const {
  bool ok(true);

  for (auto i:m_vectorOfDefinitions) {
    // std::cout<<i.str()<<std::endl;
    if (i.empty()) {
      continue;
    }
    if (not i.validType()) {
      continue;
    }
    ok &= (usersmap.insert(std::pair<std::string, SingleHistogramDefinition>(i.stringIndex(), i))).second;
  }
  return(ok and(not usersmap.empty()));
}

bool
ReadFromXml::insertDefinition(const SingleHistogramDefinition& /*oneDefinition*/) {
  bool ok(true);

  // m_vectorOfDefinitions.push_back(oneDefinition);
  return ok;
}

bool
ReadFromXml::initialize() {
  bool ok(true);

  if (m_source.empty() or(not sourceExists())) {
    throw std::runtime_error("Could not open file in ReadFromXml initialize");
  }
  ;
  myXerces::Lib xercesFrame; // RAII xerces context
  std::unique_ptr<xercesc::SAX2XMLReader> parser(xercesc::XMLReaderFactory::createXMLReader());
  // using SAX2 API, so register content and error handlers
  HDefContentHandler handler(m_vectorOfDefinitions);
  XmlErrorHandler errHandler;
  parser->setContentHandler(&handler);
  parser->setErrorHandler(&errHandler);
  // open file and parse
  parser->parse(m_source.c_str());
  return ok;
}

bool
ReadFromXml::sourceExists() const {
  struct stat buffer;

  return(stat(m_source.c_str(), &buffer) == 0);
}
