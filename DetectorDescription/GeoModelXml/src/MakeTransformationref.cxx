/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//    Process transformationref items: basically, just find the referenced transform and call its processor.
//
#include "GeoModelXml/MakeTransformationref.h"
#include "GeoModelXml/OutputDirector.h"
#include <string>
#include <xercesc/dom/DOM.hpp>
#include "GeoModelXml/GmxUtil.h"
#include "GeoModelXml/GeoNodeList.h"
#include "xercesc/util/XMLString.hpp"

using namespace std;
using namespace xercesc;

const RCBase *MakeTransformationref::make(const DOMElement *element, GmxUtil &gmxUtil) const {
const XMLCh *ref = XMLString::transcode("ref");
const XMLCh *idref;
DOMDocument *doc = element->getOwnerDocument();
char *toRelease;
//
//    Get the referenced element
//
    idref = element->getAttribute(ref);
    DOMElement *elem = doc->getElementById(idref);
//
//    Check it is the right sort
//
    toRelease = XMLString::transcode(elem->getNodeName());
    string nodeName(toRelease);
    XMLString::release(&toRelease);
    if (nodeName != string("transformation")) {
        OUTPUT_STREAM;
        msglog << MSG::FATAL << "Error in xml/gmx file: transformationref " << XMLString::transcode(idref) << " referenced a " << nodeName << 
                " instead of a transformation." << endmsg;
        exit(999); // Should do better...
    }
//
//    Process it
//
    return gmxUtil.tagHandler.transformation.process(elem, gmxUtil);
}
