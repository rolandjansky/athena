/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/MakeTransformation.h"
#include <xercesc/dom/DOM.hpp>
//   #include <CLHEP/Geometry/Transform3D.h>
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelXml/translate.h"
#include "GeoModelXml/GmxUtil.h"

#include <Eigen/Dense>

using namespace xercesc;
using namespace std;

MakeTransformation::MakeTransformation() {}

const RCBase * MakeTransformation::make(const xercesc::DOMElement *element, GmxUtil &gmxUtil) const {
char *name2release;
typedef Eigen::Affine3d Transform3D;
 Transform3D hepTransform=Transform3D::Identity(); // Starts as Identity transform
//
//   Add my element contents
//
    for (DOMNode *child = element->getFirstChild(); child != 0; child = child->getNextSibling()) {
        if (child->getNodeType() == DOMNode::ELEMENT_NODE) {
            name2release = Translate(child->getNodeName());
            string name(name2release);
            DOMElement *el = dynamic_cast<DOMElement *>(child);
            if (name == string("translation")) {
                hepTransform = hepTransform * gmxUtil.tagHandler.translation.getTransform(el, gmxUtil); 
            }
            else if (name == "rotation") {
                hepTransform = hepTransform * gmxUtil.tagHandler.rotation.getTransform(el, gmxUtil); 
            }
            else if (name == "scaling") {
	    
// TODO: figure out what to do in this case 	    
//                hepTransform = hepTransform * gmxUtil.tagHandler.scaling.getTransform(el, gmxUtil); 
            }
            XMLString::release(&name2release);
        }
    }
//
//    Create and return GeoModel transform
//
    char *toRelease = Translate(element->getAttribute(Translate("alignable")));
    string alignable(toRelease);
    XMLString::release(&toRelease);
    if (alignable.compare(string("true")) == 0) {
        return (const RCBase *) new GeoAlignableTransform(hepTransform);
    }
    else {
        return (const RCBase *) new GeoTransform(hepTransform);
    }
}
