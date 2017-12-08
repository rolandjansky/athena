/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsXMLParser/AddMaterialFactory.h"
#include "xercesc/dom/DOMNode.hpp"
#include "xercesc/dom/DOMNamedNodeMap.hpp"
#include "xercesc/dom/DOMNodeList.hpp"
#include <string>
#include <stdexcept>
#include <map>
#include "FadsMaterial/FadsElement.h"
#include "FadsMaterial/FadsMaterial.h"
#include "FadsMaterial/FadsMixture.h"
#include "FadsMaterial/FadsMolecule.h"
#include "FadsXMLParser/XMLReaderUtil.h"

#include "CLHEP/Units/PhysicalConstants.h"

namespace FADS {

  AddMaterialFactory::AddMaterialFactory(std::string s):DescriptionFactory(s)
  {
  }

  void AddMaterialFactory::Build(const DOMNode& s)
  {
    // std::cout<<" AddMaterial"<<std::endl;
    // std::cout<<s<<std::endl;
    short typ=s.getNodeType();
    const short ntags=1;
    static std::string tags[]={"material"};
    std::map<std::string,std::string,std::less<std::string> > aMap;
    for (int i=0;i<ntags;i++)
      aMap[tags[i]]="";
    if (typ==DOMNode::ELEMENT_NODE)
      {
        DOMNamedNodeMap* atts= s.getAttributes();
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
        // std::cout<<" AddMaterial: adding "<<aMap["material"]<<" to current"<<std::endl;

      }
    DOMNodeList *list=s.getChildNodes();
    for (unsigned int i=0;i<list->getLength();i++)
      {
        // std::cout<<list->item(i)<<std::endl;
        if (list->item(i)->getNodeType()==DOMNode::ELEMENT_NODE)
          {
            std::string nn=XMLReaderUtil::Transcode(list->item(i)->getNodeName());
            if (nn=="natoms")
              {
                FadsSimpleMaterial *sm=
                  FadsSimpleMaterial::GetCurrentMaterial();
                FadsMolecule *ml=dynamic_cast<FadsMolecule *>(sm);
                if (!ml) // oops, it is still a SM
                  {
                    FadsSimpleMaterial *temp=sm;
                    ml=new FadsMolecule(sm->name(),sm->density());
                    delete temp;
                  }
                DOMNamedNodeMap *atts=list->item(i)->getAttributes();
                std::string str=XMLReaderUtil::Transcode(atts->item(0)->getNodeValue());
		char *endptr;
                int na=strtol(str.c_str(), &endptr, 0);
		if (endptr[0] != '\0') {
		  throw std::invalid_argument("Could not convert string to int: " + str);	
		}
		

                ml->AddElement(XMLReaderUtil::trim(aMap["material"]),na);
              }
            else if (nn=="fractionmass")
              {
                // std::cout<<" this is a mixture"<<std::endl;
                FadsSimpleMaterial *sm=
                  FadsSimpleMaterial::GetCurrentMaterial();
                FadsMixture *ml=dynamic_cast<FadsMixture *>(sm);
                if (!ml) // oops, it is still a SM
                  {
                    // std::cout<<"oops, deleting the old one"<<std::endl;
                    FadsSimpleMaterial *temp=sm;
                    ml=new FadsMixture(sm->name(),sm->density());
                    delete temp;
                  }
                DOMNamedNodeMap *atts=list->item(i)->getAttributes();
                std::string str=XMLReaderUtil::Transcode(atts->item(0)->getNodeValue());
                float fr=atof(str.c_str());
                // std::cout<<" fraction "<<fr<<std::endl;
                ml->AddMaterial(XMLReaderUtil::trim(aMap["material"]),fr);
              }
          }
      }
  }

} // end namespace
