/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsXMLParser/CompositeFactory.h"
#include "xercesc/dom/DOMNode.hpp"
#include "xercesc/dom/DOMNamedNodeMap.hpp"
#include "xercesc/dom/DOMNodeList.hpp"
#include <string>
#include <map>
#include "FadsMaterial/FadsElement.h"
#include "FadsMaterial/FadsMaterial.h"
#include "FadsMaterial/FadsMixture.h"
#include "FadsMaterial/FadsMolecule.h"
#include "FadsXMLParser/XMLReaderUtil.h"

#include "CLHEP/Units/PhysicalConstants.h"

namespace FADS {

  CompositeFactory::CompositeFactory(std::string s):DescriptionFactory(s)
  {
  }

  void CompositeFactory::Build(const DOMNode& s)
  {
    short typ=s.getNodeType();
    const short ntags=2;
    static std::string tags[]={"name","density"};
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

        double density_f=(double)atof(aMap["density"].c_str())*CLHEP::g/CLHEP::cm3;
        FadsSimpleMaterial *mat __attribute__((unused)) =new FadsSimpleMaterial(XMLReaderUtil::trim(aMap["name"]),
                                                                                density_f);

      }
  }

} // end namespace
