/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/MakeTransformation.h"
#include <xercesc/dom/DOM.hpp>
#include <CLHEP/Geometry/Transform3D.h>
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelXml/translate.h"
#include "GeoModelXml/GmxUtil.h"

using namespace xercesc;
using namespace HepGeom;
using namespace std;

MakeTransformation::MakeTransformation() {}

const RCBase * MakeTransformation::make(const xercesc_3_1::DOMElement *element, GmxUtil &gmxUtil) const {
char *name2release;
Transform3D hepTransform; // Starts as Identity transform
//
//   Add my element contents
//
    for (DOMNode *child = element->getFirstChild(); child != 0; child = child->getNextSibling()) {
        if (child->getNodeType() == DOMNode::ELEMENT_NODE) {
            name2release = translate(child->getNodeName());
            string name(name2release);
            DOMElement *el = dynamic_cast<DOMElement *>(child);
            if (name == string("translation")) {
                hepTransform = hepTransform * gmxUtil.tagHandler.translation.getTransform(el, gmxUtil); 
            }
            else if (name == "rotation") {
                hepTransform = hepTransform * gmxUtil.tagHandler.rotation.getTransform(el, gmxUtil); 
            }
            else if (name == "scaling") {
                hepTransform = hepTransform * gmxUtil.tagHandler.scaling.getTransform(el, gmxUtil); 
            }
            XMLString::release(&name2release);
        }
    }
//
//    Create and return GeoModel transform
//
    char *toRelease = translate(element->getAttribute(translate("alignable")));
    string alignable(toRelease);
    XMLString::release(&toRelease);
    if (alignable.compare(string("true")) == 0) {
        return (const RCBase *) new GeoAlignableTransform(hepTransform);
    }
    else {
        return (const RCBase *) new GeoTransform(hepTransform);
    }
}
