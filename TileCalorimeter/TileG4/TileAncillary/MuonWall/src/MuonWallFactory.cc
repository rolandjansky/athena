/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// DescriptionFactory class for Muon Wall
//
// Author: Franck Martin <Franck.Martin@cern.ch>
// January 12, 2004
//
//************************************************************

#include "MuonWall/MuonWallFactory.hh"
#include "MuonWall/MuonWallDescription.hh"
#include "xercesc/dom/DOMNode.hpp"
#include "xercesc/dom/DOMNamedNodeMap.hpp"
#include "xercesc/dom/DOMNodeList.hpp"
#include "FadsXMLParser/XMLReaderUtil.h"
#include "TileG4DetDescr/DetectorDescriptionStore.h"
#include <string>
#include <map>

extern std::vector<std::string> tokenize(std::string);
extern std::string itos(int);

static MuonWallFactory muonWallFactory("MuonWall");

MuonWallFactory::MuonWallFactory(std::string factoryName):
  DescriptionFactory(factoryName)
{
}

void MuonWallFactory::Build(const DOMNode& s)
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

    MuonWallDescription *descMuonWall = new MuonWallDescription(XMLReaderUtil::trim(tag_values["name"]));

    descMuonWall->zLength=atof(XMLReaderUtil::trim(tag_values["ZLength"]).c_str());
    descMuonWall->yLength=atof(XMLReaderUtil::trim(tag_values["YLength"]).c_str());
    descMuonWall->xLength=atof(XMLReaderUtil::trim(tag_values["XLength"]).c_str());

  }
}


