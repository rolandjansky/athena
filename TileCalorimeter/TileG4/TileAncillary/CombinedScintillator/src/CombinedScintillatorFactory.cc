/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// DescriptionFactory class for Scintillator between LAr/Tile
//
// Author: Franck Martin <Franck.Martin@cern.ch>
// January 12, 2004
//
//************************************************************

#include "CombinedScintillator/CombinedScintillatorFactory.hh"
#include "CombinedScintillator/CombinedScintillatorDescription.hh"
#include "xercesc/dom/DOMNode.hpp"
#include "xercesc/dom/DOMNamedNodeMap.hpp"
#include "xercesc/dom/DOMNodeList.hpp"
#include "FadsXMLParser/XMLReaderUtil.h"
#include "TileG4DetDescr/DetectorDescriptionStore.h"
#include <string>
#include <map>

extern std::vector<std::string> tokenize(std::string);
extern std::string itos(int);

static CombinedScintillatorFactory combinedScintillatorFactory("CombinedScintillator");

CombinedScintillatorFactory::CombinedScintillatorFactory(std::string factoryName):
  DescriptionFactory(factoryName)
{
}

void CombinedScintillatorFactory::Build(const DOMNode& s)
{
  short node_type = s.getNodeType();

  static std::string tags[]={"name",
			     "rMin",
			     "rMax",
			     "dzSci",
                             "phiPos",
                             "phiNeg",
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

    CombinedScintillatorDescription *descCombinedScintillator = new CombinedScintillatorDescription(XMLReaderUtil::trim(tag_values["name"]));

    
descCombinedScintillator->rMin=atof(XMLReaderUtil::trim(tag_values["rMin"]).c_str());
    
descCombinedScintillator->rMax=atof(XMLReaderUtil::trim(tag_values["rMax"]).c_str());
    
descCombinedScintillator->dzSci=atof(XMLReaderUtil::trim(tag_values["dzSci"]).c_str());

descCombinedScintillator->phiPos=atof(XMLReaderUtil::trim(tag_values["phiPos"]).c_str());

descCombinedScintillator->phiNeg=atof(XMLReaderUtil::trim(tag_values["phiNeg"]).c_str());

  }
}


