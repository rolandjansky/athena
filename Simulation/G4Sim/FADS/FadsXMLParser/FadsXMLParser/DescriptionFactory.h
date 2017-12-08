/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DescriptionFactory_H
#define DescriptionFactory_H

#include "xercesc/dom/DOMNode.hpp"

XERCES_CPP_NAMESPACE_USE

#include <map>

#include <string>
#include <vector>
#include <iostream>
#include "FadsXMLParser/XMLReaderUtil.h"

namespace FADS {

  class DetectorDescription;

  class DescriptionFactory {
  private:
    int nattributes;
    std::string name;
    std::map<std::string,std::string,std::less<std::string> > attributeMap;
  public:
    DescriptionFactory(std::string n);
    virtual ~DescriptionFactory() {}
    std::string GetName();
    virtual void Build(const DOMNode& n);
    virtual void TerminateNode(const DOMNode& n);
    virtual void BuildDescription() {}
    virtual void Terminate() {}
    int NrOfAttributes() {return nattributes;}
    double GetAttributeAsDouble(std::string aName);
    int GetAttributeAsInt(std::string aName);
    std::vector<int> GetAttributeAsIntVector(std::string aName);
    std::vector<double> GetAttributeAsDoubleVector(std::string aName);
    std::vector<std::string> GetAttributeAsStringVector(std::string aName);
    std::string GetAttributeAsString(std::string aName);
    double GetAttributeAsDoubleAndUnit(std::string aName);
    bool CheckAttribute(std::string aName);
  };

  inline std::ostream& operator<< (std::ostream& target, const DOMNode& s)
  {
    std::string nam=XMLReaderUtil::Transcode(s.getNodeName());
    std::string val=XMLReaderUtil::Transcode(s.getNodeValue());
    short typ=s.getNodeType();
    if (typ!=DOMNode::ELEMENT_NODE) return target;
    target<<"Node: name="<<nam<<" value="<<val<<" type="<<typ<<std::endl;
    return target;
  }

} // end namespace

#endif
