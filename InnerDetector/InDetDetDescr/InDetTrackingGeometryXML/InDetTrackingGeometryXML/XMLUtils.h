/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// XMLUtil.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRACKINGGEOMETRY_XMLUTIL_H
#define INDETTRACKINGGEOMETRY_XMLUTIL_H

// STD library
#include <iostream>
#include <string>
#include <vector>
#include <map>
// XML library
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>

using namespace xercesc;

namespace InDet {
  class XMLUtils {
  public:
    XMLUtils();
    virtual ~XMLUtils();

    // AlgTool initialize and finalize methods
    bool InitializeXML();
    bool TerminateXML();
    void PrintErrorXML(const XMLException &e, std::string message);
    const char* getString(DOMNode *node);
    const char* getStringNoTrim(DOMNode *node);
    std::vector<std::string> getVectorString(DOMNode *node);
    std::vector<double> getVectorDouble(DOMNode *node);
    std::vector<int> getVectorInt(DOMNode *node);
    bool getBoolean(DOMNode *node);
    XMLCh* transcode(std::string s);

    // XML file parsing
    void parseFile(std::string filename, std::string tag, std::string section);
    // Function to overload to parse a node of an XML file
    virtual void parseNode(std::string section, DOMNode* node);

  protected:
    void releaseStrings();
    XercesDOMParser *m_ConfigFileParser;
    std::map<std::string, XMLCh *> m_Strings;
  };
}

#endif //INDETTRACKINGGEOMETRY_XMLUTIL_H

