//
//  XmlContentHandlers.hpp
//  HDef
//
//  Created by sroe on 16/03/2016.
//  Copyright © 2016 sroe. All rights reserved.
//

#ifndef XmlContentHandlers_h
#define XmlContentHandlers_h

#include <stdexcept>
#include <vector>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include "xmlUtilities.h"
#include <array>
#include <regex>
#include <cmath>
#include "InDetPhysValMonitoring/SingleHistogramDefinition.h"


unsigned int
partialHdefFromAttributes(SingleHistogramDefinition& s, const xercesc::Attributes& attrs) {
  unsigned int nFilled(0);
  // id, type and title are *possible* attributes
  XercesString histoStrings[] = {
    fromNative("id"), fromNative("type"), fromNative("title")
  };
  const XMLCh* val(nullptr);
  unsigned int idx {
    0
  };

  for (auto& h:histoStrings) {
    if ((val = attrs.getValue(h.c_str())) != 0) {
      switch (idx) {
      case 0:
        s.name = toNative(val);
        break;

      case 1:
        s.histoType = toNative(val);
        break;

      case 2:
        s.title = toNative(val);
        break;
      }
      nFilled++;
    }
    idx++;
  }
  return nFilled;
}

unsigned int
xAxisFromAttributes(SingleHistogramDefinition& s, const xercesc::Attributes& attrs) {
  unsigned int nFilled(0);
  // title, n, lo and hi are currently *necessary* attributes
  XercesString histoStrings[] = {
    fromNative("title"), fromNative("n"), fromNative("lo"), fromNative("hi")
  };
  const XMLCh* val(nullptr);
  unsigned int idx {
    0
  };

  IHistogramDefinitionSvc::axesLimits_t xaxis(std::nanf(""), std::nanf(""));

  for (auto& h:histoStrings) {
    if ((val = attrs.getValue(h.c_str())) != 0) {
      switch (idx) {
      case 0:
        s.xTitle = toNative(val);
        break;

      case 1:
        s.nBinsX = std::stof(toNative(val));
        break;

      case 2:
        xaxis.first = std::stof(toNative(val));
        break;

      case 3:
        xaxis.second = std::stof(toNative(val));
        break;
      }
      nFilled++;
    }
    idx++;
  }
  s.xAxis = xaxis;
  return nFilled;
}

unsigned int
yAxisFromAttributes(SingleHistogramDefinition& s, const xercesc::Attributes& attrs) {
  unsigned int nFilled(0);
  // title, n, lo and hi are currently *necessary* attributes
  XercesString histoStrings[] = {
    fromNative("title"), fromNative("n"), fromNative("lo"), fromNative("hi")
  };
  const XMLCh* val(nullptr);
  unsigned int idx {
    0
  };

  IHistogramDefinitionSvc::axesLimits_t yaxis(std::nanf(""), std::nanf(""));

  for (auto& h:histoStrings) {
    if ((val = attrs.getValue(h.c_str())) != 0) {
      switch (idx) {
      case 0:
        s.yTitle = toNative(val);
        break;

      case 1:
        s.nBinsY = std::stof(toNative(val));
        break;

      case 2:
        yaxis.first = std::stof(toNative(val));
        break;

      case 3:
        yaxis.second = std::stof(toNative(val));
        break;
      }
      nFilled++;
    }
    idx++;
  }
  s.yAxis = yaxis;
  return nFilled;
}

unsigned int
zAxisFromAttributes(SingleHistogramDefinition& s, const xercesc::Attributes& attrs) {
  unsigned int nFilled(0);
  // title, n, lo and hi are currently *necessary* attributes
  XercesString histoStrings[] = {
    fromNative("title"), fromNative("n"), fromNative("lo"), fromNative("hi")
  };
  const XMLCh* val(nullptr);
  unsigned int idx {
    0
  };

  IHistogramDefinitionSvc::axesLimits_t zaxis(std::nanf(""), std::nanf(""));

  for (auto& h:histoStrings) {
    if ((val = attrs.getValue(h.c_str())) != 0) {
      switch (idx) {
      case 0:
        s.zTitle = toNative(val);
        break;

      case 1:
        s.nBinsZ = std::stof(toNative(val));
        break;

      case 2:
        zaxis.first = std::stof(toNative(val));
        break;

      case 3:
        zaxis.second = std::stof(toNative(val));
        break;
      }
      nFilled++;
    }
    idx++;
  }
  s.zAxis = zaxis;
  return nFilled;
}

unsigned int
partialHdefFromText(SingleHistogramDefinition& s, const std::string& line) {
  unsigned int nFilled(0);
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
    nFilled = (int) m.size() - 1;
  }
  return nFilled;
}

unsigned int
partialHdefFromTProfText(SingleHistogramDefinition& s, const std::string& line) {
  unsigned int nFilled(0);
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
    nFilled = (int) m.size() - 1;
  }
  return nFilled;
}

class HDefContentHandler: public xercesc::DefaultHandler {
public:
  enum HistoTypes {
    UNKNOWN, TH1, TPROFILE, TH2, TH3, TEFFICIENCY, NTYPES
  };
  HDefContentHandler(std::vector<SingleHistogramDefinition>& defs) : m_histoType(UNKNOWN), m_numberFilled(0),
    m_currentDefinition{}, m_definitions(defs), m_currentText() {
    // nop
  }

  void
  startElement(const XMLCh* const /*uri*/, const XMLCh* const localName, const XMLCh* const /*qName*/,
               const xercesc::Attributes& attrs) {
    const static XercesString hdefStr = fromNative("hdef");
    const static XercesString hStr = fromNative("h");
    const static XercesString xStr = fromNative("x");
    const static XercesString yStr = fromNative("y");
    const static XercesString zStr = fromNative("z");
    unsigned int nFilled(0);

    if (localName == hStr) {
      setHistoType(attrs);
      nFilled = partialHdefFromAttributes(m_currentDefinition, attrs);
      m_currentText.clear();
    }
    if (localName == xStr) {
      nFilled = xAxisFromAttributes(m_currentDefinition, attrs);
    }
    if (localName == yStr) {
      nFilled = yAxisFromAttributes(m_currentDefinition, attrs);
    }
    if (localName == zStr) {
      nFilled = zAxisFromAttributes(m_currentDefinition, attrs);
    }
    m_currentText.clear();
    m_numberFilled += nFilled;
  }

  void
  endElement(const XMLCh* const /*uri*/, const XMLCh* const localName, const XMLCh* const /*qName*/) {
    const static XercesString hdefStr = fromNative("hdef");
    const static XercesString hStr = fromNative("h");
    const static XercesString xStr = fromNative("x");
    const static XercesString yStr = fromNative("y");

    if (localName == hStr) {
      std::string line = toNative(m_currentText);
      if (m_histoType == TH1) {
        unsigned int n = partialHdefFromText(m_currentDefinition, line);
        m_numberFilled += n;
      }
      if (m_histoType == TPROFILE) {
        unsigned int n = partialHdefFromTProfText(m_currentDefinition, line);
        m_numberFilled += n;
      }

      m_currentDefinition.m_empty = (m_numberFilled == 0);
      m_currentDefinition.allTitles = m_currentDefinition.titleDigest();
      m_definitions.push_back(m_currentDefinition);
    }
  }

  void
  characters(const XMLCh* const chars, const XMLSize_t length) {
    m_currentText.append(chars, length);
  }

private:
  HistoTypes m_histoType;
  unsigned int m_numberFilled;
  SingleHistogramDefinition m_currentDefinition;
  std::vector<SingleHistogramDefinition>& m_definitions;
  XercesString m_currentText;
  void
  setHistoType(const xercesc::Attributes& attrs) {
    m_histoType = UNKNOWN;
    const XMLCh* val(nullptr);
    const static XercesString TH1Str = fromNative("TH1F");
    const static XercesString TProfileStr = fromNative("TProfile");
    const static XercesString TH2Str = fromNative("TH2F");
    const static XercesString TH3Str = fromNative("TH3F");
    const static XercesString TEffStr = fromNative("TEfficiency");
    XercesString t = fromNative("type");
    val = attrs.getValue(t.c_str());
    if (val == 0) {
      return;
    }
    if (val == TH1Str) {
      m_histoType = TH1;
    }
    if (val == TProfileStr) {
      m_histoType = TPROFILE;
    }
    if (val == TH2Str) {
      m_histoType = TH2;
    }
    if (val == TH3Str) {
      m_histoType = TH3;
    }
    if (val == TEffStr) {
      m_histoType = TEFFICIENCY;
    }
  }
};





#endif /* XmlContentHandlers_hpp */
