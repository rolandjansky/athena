/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    const XMLCh *idref = element->getAttribute(XMLString::transcode("ref"));
    DOMDocument *doc = element->getOwnerDocument();
    DOMElement *shape = doc->getElementById(idref);
//
//    Check it is the right sort: it's parent should be a shapes
//
    DOMNode *parent = shape->getParentNode();
    if (XMLString::compareIString(parent->getNodeName(), XMLString::transcode("shapes")) != 0) {
        OUTPUT_STREAM;
        msglog << MSG::FATAL << "Error processing <shaperef> tag: An IDREF for a shape did not refer to a shape.\nShape ref was " << 
               idref << "; exiting" << endmsg;
        exit (1); // Need to improve...
    }
//
//    Get what sort of shape
//
    char *toRelease = XMLString::transcode(shape->getNodeName());
    string tag(toRelease);
    XMLString::release(&toRelease);
//
//    Find and return it
//
    return (RCBase *) gmxUtil.geoItemRegistry.find(tag)->process(shape, gmxUtil);
}
