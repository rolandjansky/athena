/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsXMLParser/ElementFactory.h"
#include "xercesc/dom/DOMNode.hpp"
#include "xercesc/dom/DOMNamedNodeMap.hpp"
#include "xercesc/dom/DOMNodeList.hpp"
#include <string>
#include <map>
#include "FadsMaterial/FadsElement.h"
#include "FadsMaterial/FadsMaterial.h"
#include "FadsXMLParser/XMLReaderUtil.h"
#include "CLHEP/Units/PhysicalConstants.h"

namespace FADS {

  ElementFactory::ElementFactory(std::string s):DescriptionFactory(s)
  {
  }

  void ElementFactory::Build(const DOMNode& s)
  {
    short typ=s.getNodeType();
    const short ntags=5;
    static std::string tags[]={"z","name","symbol","aweight","density"};
    std::map<std::string,std::string,std::less<std::string> > aMap;
    for (int i=0;i<ntags;i++)
      aMap[tags[i]]="";
    if (typ==DOMNode::ELEMENT_NODE)
      {
        DOMNamedNodeMap *atts= s.getAttributes();
        //std::string nam;
        for (unsigned int j=0;j<atts->getLength();j++)
          {
            std::string c=XMLReaderUtil::Transcode(atts->item(j)->getNodeName());
            std::string str=XMLReaderUtil::Transcode(atts->item(j)->getNodeValue());
            if (aMap.find(c)==aMap.end())
              std::cout<<"Unknown tag "<<c<<std::endl;
            else
              aMap[c]=str;
          }
        // std::cout<<" ElementFactory defining -"<<aMap["name"]<<"-"<<
        // std::endl;
        float z_f=atof(aMap["z"].c_str());
        float aweight_f=atof(aMap["aweight"].c_str())*CLHEP::g/CLHEP::mole;
        float density_f=atof(aMap["density"].c_str())*CLHEP::g/CLHEP::cm3;
        FadsElement *el __attribute__((unused)) =new FadsElement(XMLReaderUtil::trim(aMap["name"]),
                                                                 XMLReaderUtil::trim(aMap["symbol"]),
                                                                 z_f, aweight_f);
        if (density_f)
          FadsMaterial *mat __attribute__((unused)) =new FadsMaterial(XMLReaderUtil::trim(aMap["name"]),
                                                                      z_f,aweight_f, density_f);

      }

  }

}
