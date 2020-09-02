/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//
//  ReadFromXmlDom.cxx
//  HDef
//
//  Created by sroe on 15/03/2016.
//

#include "ReadFromXmlDom.h"
#include "xmlUtilities.h"
#include "XincludeErrHandler.h"
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <sys/stat.h>
#include "PathResolver/PathResolver.h"
#include <array>
#include <algorithm>
#include <cmath>
#include <regex>

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

namespace {
  bool
  validAxisName(const std::string& proposedName, const std::array<std::string, 3>& allowedNames) {
    return((proposedName == allowedNames[0])or(proposedName == allowedNames[1])or(proposedName == allowedNames[2]));
  }
}

ReadFromXmlDom::ReadFromXmlDom() : m_source("unspecified file"), m_format("text/xml") {
}

ReadFromXmlDom::ReadFromXmlDom(const std::string& source) : m_format("text/xml") {
  m_source = PathResolver::find_file(source, "DATAPATH");
}

std::string
ReadFromXmlDom::source() const {
  return m_source;
}

std::string
ReadFromXmlDom::format() const {
  return m_format;
}

bool
ReadFromXmlDom::histoDefinitionMap(std::map<std::string, SingleHistogramDefinition>& usersmap) const {
  bool ok(true);

  for (auto i:m_vectorOfDefinitions) {
    if (i.empty()) {
      continue;
    }
    if (not i.validType()) {
      continue;
    }
    bool thisIsOk = (usersmap.insert(std::pair<std::string, SingleHistogramDefinition>(i.stringIndex(), i))).second;
    if (not thisIsOk) {
      ok &= thisIsOk;
      std::string msg = "You have attempted to add a duplicate histogram definition: " + i.stringIndex();
      throw std::runtime_error(msg);
    }
  }
  return(ok and(not usersmap.empty()));
}

bool
ReadFromXmlDom::insertDefinition(const SingleHistogramDefinition& oneDefinition) {
  bool ok(true);

  m_vectorOfDefinitions.push_back(oneDefinition);
  return ok;
}

bool
ReadFromXmlDom::initialize() {
  bool ok(true);

  if (m_source.empty() or(not sourceExists())) {
    std::cerr << "Could not open file " << m_source << " in ReadFromXmlDom initialize" << std::endl;
    return false;
  }
  myXerces::Lib xercesFrame; // RAII xerces context
  static const XMLCh gLS[] = {
    xercesc::chLatin_L, xercesc::chLatin_S, xercesc::chNull
  };
  xercesc::DOMImplementation* impl = xercesc::DOMImplementationRegistry::getDOMImplementation(gLS);
  xercesc::DOMLSParser* parser = ((xercesc::DOMImplementationLS*) impl)->createLSParser(
    xercesc::DOMImplementationLS::MODE_SYNCHRONOUS, 0);
  xercesc::DOMConfiguration* config = parser->getDomConfig();
  if (config->canSetParameter(xercesc::XMLUni::fgXercesDoXInclude, true)) {
    config->setParameter(xercesc::XMLUni::fgXercesDoXInclude, true);
  }
  XIncludeErrHandler errorHandler;
  config->setParameter(xercesc::XMLUni::fgDOMErrorHandler, &errorHandler);
  auto doc = parser->parseURI(m_source.c_str());
  const XercesString temp = fromNative("h");
  xercesc::DOMNodeList* list = doc->getElementsByTagName(temp.c_str());
  const auto nElements = list->getLength();
  for (unsigned long i(0); i != nElements; ++i) {
    xercesc::DOMNode const* thisNode = list->item(i);
    auto thisElement = dynamic_cast<xercesc::DOMElement const*> (thisNode);
    if (thisElement) {
      insertDefinition(parseXmlElement(thisElement));
    }
  }
  parser->release();
  return ok;
}

bool
ReadFromXmlDom::sourceExists() const {
  struct stat buffer;

  return(stat(m_source.c_str(), &buffer) == 0);
}

SingleHistogramDefinition
ReadFromXmlDom::parseXmlElement(const xercesc::DOMElement* element) {
  SingleHistogramDefinition s;
  enum RegXHistoGroups {
    TYPE, NAME, TITLE, NX, NY, NZ, XLO, YLO, ZLO, XHI, YHI, ZHI, XAXIS, YAXIS, ZAXIS, FOLDER, NGROUPS
  };
  const std::array<std::string, NGROUPS> attrNames = {
    "type", "id", "title", "n", "n", "n", "lo", "lo", "lo", "hi", "hi", "hi", "title", "title", "title", "folder"
  };
  //
  // transform the std::string attribute names to Xerces string attribute names
  std::array<XercesString, NGROUPS> xercesNames;
  std::transform(attrNames.begin(), attrNames.end(), xercesNames.begin(), [](const std::string& s) {
    return fromNative(s);
  });
  // Use this array to store the primary returned attribute values, which will be Xerces strings
  std::array<XercesString, NGROUPS> xercesValues;
  //
  constexpr unsigned int NAXES = 3; // allow only two axes, could be extended later
  const std::array<std::string, NAXES> allowedAxisNames = {
    "x", "y", "z"
  };

  //
  xercesValues[NAME] = element->getAttribute(xercesNames[NAME].c_str());
  xercesValues[TITLE] = element->getAttribute(xercesNames[TITLE].c_str());
  xercesValues[TYPE] = element->getAttribute(xercesNames[TYPE].c_str());
  xercesValues[FOLDER] = element->getAttribute(xercesNames[FOLDER].c_str());
  const std::string type = toNative(xercesValues[TYPE]);
  const bool isTProfile = (type == "TProfile");
  // get children of the histogram, these are the two axes
  const xercesc::DOMElement* axisDef0 = element->getFirstElementChild();
  if (!axisDef0 and element->hasChildNodes()) {
    XercesString xercesContent = element->getTextContent();
    const std::string textContent = toNative(xercesContent);
    if (textContent.empty()) {
      return s;
    }
    SingleHistogramDefinition sx = isTProfile ? parseTProfileText(textContent) : parseTextLine(textContent);
    sx.name = toNative(xercesValues[NAME]);
    sx.histoType = type;
    return sx;
  }
  //if get to here and axisDef0 is null, theres a problem
  if (not axisDef0) return s;
  const xercesc::DOMElement* axisDef1 = axisDef0->getNextElementSibling();
  // only allow two axes, but could be ordered x-y or y-x
  std::string axisName0 = toNative(axisDef0->getTagName());
  std::string axisName1 = toNative(axisDef1->getTagName());
  const xercesc::DOMElement* axisDef2 = axisDef1->getNextElementSibling();
  std::string axisName2 = axisDef2 ? toNative(axisDef2->getTagName()) : "z";
  if (validAxisName(axisName0, allowedAxisNames) 
   and validAxisName(axisName1,allowedAxisNames) 
   and validAxisName(axisName2,allowedAxisNames)
   and (axisName1 != axisName0) and (axisName2 != axisName1)) {
    // default order
    unsigned int xIndex = 0, yIndex = 1, zIndex = 2;
    if (axisName0 == allowedAxisNames[1]) {
      // inverted order
      xIndex = 1;
      yIndex = 0;
    }
    xercesValues[NX + xIndex] = axisDef0->getAttribute(xercesNames[NX + xIndex].c_str());
    xercesValues[NX + yIndex] = axisDef1->getAttribute(xercesNames[NX + yIndex].c_str());
    xercesValues[NX + zIndex] = axisDef2 ? axisDef2->getAttribute(xercesNames[NX + zIndex].c_str()) : XercesString();
    xercesValues[XLO + xIndex] = axisDef0->getAttribute(xercesNames[XLO + xIndex].c_str());
    xercesValues[XLO + yIndex] = axisDef1->getAttribute(xercesNames[XLO + yIndex].c_str());
    xercesValues[XLO + zIndex] = axisDef2 ? axisDef2->getAttribute(xercesNames[XLO + zIndex].c_str()) : XercesString();
    xercesValues[XHI + xIndex] = axisDef0->getAttribute(xercesNames[XHI + xIndex].c_str());
    xercesValues[XHI + yIndex] = axisDef1->getAttribute(xercesNames[XHI + yIndex].c_str());
    xercesValues[XHI + zIndex] = axisDef2 ? axisDef2->getAttribute(xercesNames[XHI + zIndex].c_str()) : XercesString();
    xercesValues[XAXIS + xIndex] = axisDef0->getAttribute(xercesNames[XAXIS + xIndex].c_str());
    xercesValues[XAXIS + yIndex] = axisDef1->getAttribute(xercesNames[XAXIS + yIndex].c_str());
    xercesValues[XAXIS + zIndex] = axisDef2 ? axisDef2->getAttribute(xercesNames[XAXIS + zIndex].c_str()) : XercesString();
    // transform Xerces strings to normal std::string
    std::array<std::string, NGROUPS> stringValues {
      ""
    };
    std::transform(xercesValues.begin(), xercesValues.end(), stringValues.begin(), [](const XercesString& s) {
      return toNative(s);
    });
    // numerical values are required for some quantities
    const float NaN = std::nanf(""); // default 'invalid' float is not-a-number (NaN)
    const unsigned int nx = stringValues[NX].empty() ? 0 : (unsigned int) (std::stoul(stringValues[NX]));
    const unsigned int ny = stringValues[NY].empty() ? 0 : (unsigned int) (std::stoul(stringValues[NY]));
    const unsigned int nz = stringValues[NZ].empty() ? 0 : (unsigned int) (std::stoul(stringValues[NZ]));
    const float xlo = stringValues[XLO].empty() ? NaN : std::stof(stringValues[XLO]);
    const float ylo = stringValues[YLO].empty() ? NaN : std::stof(stringValues[YLO]);
    const float zlo = stringValues[ZLO].empty() ? NaN : std::stof(stringValues[ZLO]);
    const float xhi = stringValues[XHI].empty() ? NaN : std::stof(stringValues[XHI]);
    const float yhi = stringValues[YHI].empty() ? NaN : std::stof(stringValues[YHI]);
    const float zhi = stringValues[ZHI].empty() ? NaN : std::stof(stringValues[ZHI]);
    // now build the histogram definition to return
    SingleHistogramDefinition sx(stringValues[NAME], stringValues[TYPE], stringValues[TITLE], 
				 nx, ny, nz, xlo, xhi, ylo, yhi, zlo, zhi,
				 stringValues[XAXIS], stringValues[YAXIS], stringValues[ZAXIS],
                                 stringValues[FOLDER]);
    return sx;
  }
  return s;
}

SingleHistogramDefinition
ReadFromXmlDom::parseTextLine(const std::string& line) {
  SingleHistogramDefinition s;
  enum RegXHistoGroups {
    TOTAL, TITLE, NBINS, XLO, XHI, XAXIS, YAXIS, DUMMY, FOLDER, NGROUPS
  };

  std::string rex =
    R"delim(^\s+"([^"]+)"\s+(\d+)\s+([-+.0-9eE]+)\s+([-+.0-9eE]+)\s+"([^"]+)"\s+"([^"]+)"\s*(.*)\s*$)delim";
  std::regex reg(rex);
  std::smatch m;

  if (std::regex_match(line, m, reg)) {
    const bool hasFolder = (m.size() == NGROUPS);
    s.title = m[TITLE].str();
    s.xTitle = m[XAXIS].str();
    s.yTitle = m[YAXIS].str();
    s.xAxis = IHistogramDefinitionSvc::axesLimits_t(std::stof(m[XLO].str()), std::stof(m[XHI].str()));
    s.nBinsX = std::stoi(m[NBINS].str());
    if (hasFolder) {
      s.folder = m[FOLDER].str();
    }
  }
  s.m_empty = false;
  return s;
}

SingleHistogramDefinition
ReadFromXmlDom::parseTProfileText(const std::string& line) {
  SingleHistogramDefinition s;
  enum RegXHistoGroups {
    TOTAL, TITLE, NBINS, XLO, XHI, YLO, YHI, XAXIS, YAXIS, DUMMY, FOLDER, NGROUPS
  };

  // text like: &quot;Test of TProfile&quot; 20 -50 50 0 200 &quot;#eta&quot;  &quot;testEntries&quot;
  std::string rex =
    R"delim(^\s+"([^"]+)"\s+(\d+)\s+([-+.0-9eE]+)\s+([-+.0-9eE]+)\s+([-+.0-9eE]+)\s+([-+.0-9eE]+)\s+"([^"]+)"\s+"([^"]+)"\s*(.*)\s*$)delim";
  std::regex reg(rex);
  std::smatch m;

  if (std::regex_match(line, m, reg)) {
    const bool hasFolder = (m.size() == NGROUPS);
    s.title = m[TITLE].str();
    s.xTitle = m[XAXIS].str();
    s.yTitle = m[YAXIS].str();
    s.xAxis = IHistogramDefinitionSvc::axesLimits_t(std::stof(m[XLO].str()), std::stof(m[XHI].str()));
    s.yAxis = IHistogramDefinitionSvc::axesLimits_t(std::stof(m[YLO].str()), std::stof(m[YHI].str()));
    s.nBinsX = std::stoi(m[NBINS].str());
    if (hasFolder) {
      s.folder = m[FOLDER].str();
    }
  }
  s.m_empty = false;
  return s;
}
