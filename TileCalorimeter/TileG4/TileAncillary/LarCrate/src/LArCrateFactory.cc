/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//************************************************************
//
// DescriptionFactory class for LAr Crate
//
// Author: Franck Martin <Franck.Martin@cern.ch>
// November 19, 2004
//
//************************************************************

#include "LarCrate/LArCrateFactory.hh"
#include "LarCrate/LArCrateDescription.hh"
#include "xercesc/dom/DOMNode.hpp"
#include "xercesc/dom/DOMNamedNodeMap.hpp"
#include "xercesc/dom/DOMNodeList.hpp"
#include "FadsXMLParser/XMLReaderUtil.h"
#include "TileG4DetDescr/DetectorDescriptionStore.h"
#include <string>
#include <map>

extern std::vector<std::string> tokenize(std::string);
extern std::string itos(int);

static LArCrateFactory larCrateFactory("LArCrate");

LArCrateFactory::LArCrateFactory(std::string factoryName):
  DescriptionFactory(factoryName)
{
}

void LArCrateFactory::Build(const DOMNode& s)
{
  short node_type = s.getNodeType();

  static std::string tags[]={"name",
			     "ZLength",
			     "YLength",
			     "XLength",
			     };

  const short ntags=sizeof(tags)/sizeof(std::string);
  std::map<std::string,std::string,std::less<std::string> > tag_values;

  //Initialize Tag values with empty strings
  for (int i=0;i<ntags;i++) tag_values[tags[i]]="";

  if (node_type == DOMNode::ELEMENT_NODE)
  {
    DOMNamedNodeMap* atts= s.getAttributes();
		                                                                                                      
    for (int j=0; j<static_cast<int>(atts->getLength()); j++)
    {
      std::string att_name = XMLReaderUtil::Transcode(atts->item(j)->getNodeName());
      std::string att_value = XMLReaderUtil::Transcode(atts->item(j)->getNodeValue());

      if (tag_values.find(att_name) == tag_values.end())
      {
        std::cout<<"Unknown attribute " << att_name << std::endl;
      }
      else
      {
        tag_values[att_name] = att_value;
      }
    }	

    LArCrateDescription *descLArCrate = new LArCrateDescription(XMLReaderUtil::trim(tag_values["name"]));

    descLArCrate->zLength=atof(XMLReaderUtil::trim(tag_values["ZLength"]).c_str());
    descLArCrate->yLength=atof(XMLReaderUtil::trim(tag_values["YLength"]).c_str());
    descLArCrate->xLength=atof(XMLReaderUtil::trim(tag_values["XLength"]).c_str());

  }
}


