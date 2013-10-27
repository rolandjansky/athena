/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// DescriptionFactory class for Dead Material
//
// Author: Franck Martin <Franck.Martin@cern.ch>
// July 12, 2004
//
//************************************************************

#include "DeadMaterial/DeadMaterialFactory.hh"
#include "DeadMaterial/DeadMaterialDescription.hh"
#include "xercesc/dom/DOMNode.hpp"
#include "xercesc/dom/DOMNamedNodeMap.hpp"
#include "xercesc/dom/DOMNodeList.hpp"
#include "FadsXMLParser/XMLReaderUtil.h"
#include "TileG4DetDescr/DetectorDescriptionStore.h"
#include <string>
#include <map>

extern std::vector<std::string> tokenize(std::string);
extern std::string itos(int);

static DeadMaterialFactory deadMaterialFactory("DeadMaterial");

DeadMaterialFactory::DeadMaterialFactory(std::string factoryName):
  DescriptionFactory(factoryName)
{
}

void DeadMaterialFactory::Build(const DOMNode& s)
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

    DeadMaterialDescription *descDeadMaterial = new DeadMaterialDescription(XMLReaderUtil::trim(tag_values["name"]));

    descDeadMaterial->zLength=atof(XMLReaderUtil::trim(tag_values["ZLength"]).c_str());
    descDeadMaterial->yLength=atof(XMLReaderUtil::trim(tag_values["YLength"]).c_str());
    descDeadMaterial->xLength=atof(XMLReaderUtil::trim(tag_values["XLength"]).c_str());

  }
}
