/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Automatically generated code from /home/hessey/prog/gmx2geo/makeshape
// But then edited for AddPlane stuff
#include "GeoModelXml/shape/MakePcon.h"
#include <xercesc/dom/DOM.hpp>
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelXml/translate.h"
#include "GeoModelXml/GmxUtil.h"

using namespace xercesc;

MakePcon::MakePcon() {}

const RCBase * MakePcon::make(const xercesc::DOMElement *element, GmxUtil &gmxUtil) const {
const int nParams = 2; 
char const *parName[nParams] = {"sphi", "dphi"};
double p[nParams];
char *toRelease;

    for (int i = 0; i < nParams; ++i) {
        toRelease = Translate(element->getAttribute(Translate(parName[i])));
        p[i] = gmxUtil.evaluate(toRelease);
        XMLString::release(&toRelease);
    }

    GeoPcon *pcon = new GeoPcon(p[0], p[1]);
//
//    Add planes
//
    double zPlane = 0.;
    double rMinPlane = 0.;
    double rMaxPlane = 0.;
    for (DOMNode *child = element->getFirstChild(); child != 0; child = child->getNextSibling()) {
        if (child->getNodeType() == DOMNode::ELEMENT_NODE) {
            toRelease = Translate(child->getNodeName());
            std::string name(toRelease);
            XMLString::release(&toRelease);
            if (name == "addplane") {
                gmxUtil.tagHandler.addplane.process(dynamic_cast<const DOMElement *>(child), zPlane, rMinPlane, rMaxPlane);
                pcon->addPlane(zPlane, rMinPlane, rMaxPlane);
            }
        }
    }

    return pcon;
}
