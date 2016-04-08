/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsXMLParser/DescriptionFactory.h"
#include "FadsXMLParser/XMLReaderUtil.h"
#include "FadsXMLParser/XMLReader.h"
#include "FadsUtilities/Tokenizer.h"

#include <stdexcept>

#include "G4UnitsTable.hh"

namespace FADS {

  std::string DescriptionFactory::GetName()
  {
    return name;
  }
  DescriptionFactory::DescriptionFactory(std::string n):nattributes(0),name(n)
  {
    XMLReader::GetXMLReader()->AddFactory(this);
  }

  void DescriptionFactory::Build(const DOMNode& s)
  {
    short typ=s.getNodeType();
    //const short ntags=2;
    if (typ==DOMNode::ELEMENT_NODE)
      {
        DOMNamedNodeMap *atts= s.getAttributes();
        std::string nam;
        for (unsigned int j=0;j<atts->getLength();j++)
          {
            std::string c=XMLReaderUtil::Transcode(atts->item(j)->getNodeName());
            //int typ=atts->item(j)->getNodeType();

            std::string str=XMLReaderUtil::Transcode(atts->item(j)->getNodeValue());
            // cout<<" attribute "<<c<<" type "<<typ<<" "<<str<<endl;
            attributeMap[c]=str;

          }
      }
    BuildDescription();
  }

  void DescriptionFactory::TerminateNode(const DOMNode& /*s*/)
  {
    Terminate();
  }

  double DescriptionFactory::GetAttributeAsDouble(std::string aName)
  {
    if (attributeMap.find(aName)==attributeMap.end())
      return 0;
    else
      return (double)atof(attributeMap[aName].c_str());
  }
  int DescriptionFactory::GetAttributeAsInt(std::string aName)
  {
    if (attributeMap.find(aName)==attributeMap.end())
      return 0;
    char *endptr;
    int result = strtol(attributeMap[aName].c_str(), &endptr, 0);
    if (endptr[0] != '\0') {
      throw std::invalid_argument("Could not convert string to int: " + aName);	
    }
    return result;
  }
  std::string DescriptionFactory::GetAttributeAsString(std::string aName)
  {
    static std::string nullstring="";
    if (attributeMap.find(aName)==attributeMap.end())
      return nullstring;
    else
      return attributeMap[aName];
  }

  std::vector<int> DescriptionFactory::GetAttributeAsIntVector(std::string aName)
  {
    std::vector<int> out;
    if (attributeMap.find(aName)==attributeMap.end())
      return out;
    else
      {
        std::string blank(" ");
        FADS::Tokenizer t(blank,attributeMap[aName]);
        typedef std::vector<std::string>::const_iterator Itr;
	char *endptr;
        for (Itr p=t.begin(); p!=t.end(); p++)
          {
            if ((*p)!=" " && !(*p).empty()) {
              out.push_back(strtol((*p).c_str(), &endptr, 0));
	      if (endptr[0] != '\0') {
		throw std::invalid_argument("Could not convert string to int: " + (*p));	
	      }
	    }
          }
        return out;
      }
  }
  std::vector<double> DescriptionFactory::GetAttributeAsDoubleVector(std::string aName)
  {
    std::vector<double> out;
    if (attributeMap.find(aName)==attributeMap.end())
      return out;
    else
      {
        std::string blank(" ");
        FADS::Tokenizer t(blank,attributeMap[aName]);
        typedef std::vector<std::string>::const_iterator Itr;
        for (Itr p=t.begin(); p!=t.end(); p++)
          {
            if ((*p)!=" " && !(*p).empty()) out.push_back(atof((*p).c_str()));
          }
        return out;
      }
  }
  bool DescriptionFactory::CheckAttribute(std::string aName)
  {
    return (attributeMap.find(aName)!=attributeMap.end());
  }
  std::vector<std::string> DescriptionFactory::GetAttributeAsStringVector(std::string aName)
  {
    std::vector<std::string> out;
    if (attributeMap.find(aName)==attributeMap.end())
      return out;
    else
      {
        std::string blank(" ");
        FADS::Tokenizer t(blank,attributeMap[aName]);
        typedef std::vector<std::string>::const_iterator Itr;
        for (Itr p=t.begin(); p!=t.end(); p++)
          {
            if ((*p)!=" " && !(*p).empty()) out.push_back(*p);
          }
        return out;
      }
  }


  double DescriptionFactory::GetAttributeAsDoubleAndUnit(std::string aName)
  {
    std::vector<std::string> vstr=GetAttributeAsStringVector(aName);
    if (vstr.size()!=2)
      {
        std::cout<<" GetAttributeAsDoubleAndUnit: "
                 <<" something is wrong, size is "<<vstr.size()
                 <<std::endl;
        std::cout<<" Attribute "<<aName<<std::endl;
      }
    double temp=atof(vstr[0].c_str());
    double temp2;
    if (vstr[1]=="%") temp2=0.01;
    else temp2=G4UnitDefinition::GetValueOf(vstr[1]);
    return temp*temp2;
  }

} // end namespace
