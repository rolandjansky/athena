/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XMLReader_H
#define XMLReader_H


#include <stdlib.h>
#include <string>
#include <map>
#include <iostream>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
XERCES_CPP_NAMESPACE_USE

#include "FadsXMLParser/DescriptionFactory.h"

namespace FADS {

  typedef std::map<std::string,DescriptionFactory*,std::less<std::string> > factoryMap;


  class XMLReader {
    friend class DescriptionFactory;
  private:
    XMLReader();
    static XMLReader* thePointer;
    static factoryMap theMap;
    void AddFactory(DescriptionFactory *f);
  public:
    DescriptionFactory *GetFactory(std::string n);
    static XMLReader* GetXMLReader();
    // static std::string& Transcode(const DOMString& t);
    XMLReader(std::string name);
    ~XMLReader();
    void Process(std::string name);
    void ProcessNode(const DOMNode& s);
    // static std::string trim (const std::string s);
  };

} // end namespace

#endif
