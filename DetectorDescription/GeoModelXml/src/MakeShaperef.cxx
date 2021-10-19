/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//
//    Process transformationref items: basically, just find the referenced transform and call its processor.
//
#include "GeoModelXml/shape/MakeShaperef.h"
#include "GeoModelXml/OutputDirector.h"
#include <string>
#include <xercesc/dom/DOM.hpp>
#include "GeoModelXml/GmxUtil.h"
#include "xercesc/util/XMLString.hpp"

using namespace std;
using namespace xercesc;

const RCBase *MakeShaperef::process(const DOMElement *element, GmxUtil &gmxUtil) const {
//
//    Get the referenced shape
//

    XMLCh * ref_tmp = XMLString::transcode("ref");
    XMLCh * shapes_tmp = XMLString::transcode("shapes");

    const XMLCh *idref = element->getAttribute(ref_tmp);
    DOMDocument *doc = element->getOwnerDocument();
    DOMElement *shape = doc->getElementById(idref);
//
//    Check it is the right sort: it's parent should be a shapes
//
    DOMNode *parent = shape->getParentNode();
    if (XMLString::compareIString(parent->getNodeName(), shapes_tmp) != 0) {
        char* idref_s = XMLString::transcode (idref);
        OUTPUT_STREAM;
        msglog << MSG::FATAL << "Error processing <shaperef> tag: An IDREF for a shape did not refer to a shape.\nShape ref was " << 
               idref_s << "; exiting" << endmsg;
        XMLString::release (&idref_s);
        exit (1); // Need to improve...
    }
//
//    Get what sort of shape
//
    char *toRelease = XMLString::transcode(shape->getNodeName());
    string tag(toRelease);
    XMLString::release(&toRelease);
    XMLString::release(&ref_tmp);
    XMLString::release(&shapes_tmp);
//
//    Find and return it
//
    return (RCBase *) gmxUtil.geoItemRegistry.find(tag)->process(shape, gmxUtil);
}
