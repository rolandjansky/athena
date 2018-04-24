/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//    Process transformationref items: basically, just find the referenced transform and call its processor.
//
#include "GeoModelXml/shape/MakeShaperef.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include <string>
#include <xercesc/dom/DOM.hpp>
#include "GeoModelXml/GmxUtil.h"
#include "GeoModelXml/translate.h"

using namespace std;
using namespace xercesc;

const RCBase *MakeShaperef::process(const DOMElement *element, GmxUtil &gmxUtil) const {
//
//    Get the referenced shape
//
    const XMLCh *idref = element->getAttribute(translate("ref"));
    DOMDocument *doc = element->getOwnerDocument();
    DOMElement *shape = doc->getElementById(idref);
//
//    Check it is the right sort: it's parent should be a shapes
//
    DOMNode *parent = shape->getParentNode();
    if (XMLString::compareIString(parent->getNodeName(), translate("shapes")) != 0) {
        ServiceHandle<IMessageSvc> msgh("MessageSvc", "GeoModelXml");
        MsgStream log(&(*msgh), "GeoModelXml");
        log << MSG::FATAL << "Error processing <shaperef> tag: An IDREF for a shape did not refer to a shape.\nShape ref was " << 
               idref << "; exiting" << endmsg;
        exit (1); // Need to improve...
    }
//
//    Get what sort of shape
//
    char *toRelease = translate(shape->getNodeName());
    string tag(toRelease);
    XMLString::release(&toRelease);
//
//    Find and return it
//
    return (RCBase *) gmxUtil.geoItemRegistry.find(tag)->process(shape, gmxUtil);
}
