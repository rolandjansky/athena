/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//    Process logvolref items: basically, just find the referenced logvol and call its processor.
//
#include "GeoModelXml/LogvolrefProcessor.h"
#include "GeoModelXml/OutputDirector.h"
#include <string>
#include <xercesc/dom/DOM.hpp>
#include "GeoModelXml/GeoNodeList.h"
#include "xercesc/util/XMLString.hpp"
#include "GeoModelXml/GmxUtil.h"

using namespace std;
using namespace xercesc;

void LogvolrefProcessor::process(const DOMElement *element, GmxUtil &gmxUtil, GeoNodeList &toAdd) {
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
    if (nodeName != string("logvol")) {
	OUTPUT_STREAM;
        msglog << MSG::FATAL << "Error in xml/gmx file: logvolref " << XMLString::transcode(idref) << " referenced a " << nodeName << 
                " instead of a logvol." << endmsg;
        exit(999); // Should do better...
    }
//
//    Process it
//
    const XMLCh *zeroid = element->getAttribute(XMLString::transcode("zeroid"));
    if (XMLString::equals(zeroid, XMLString::transcode("true"))) {
        gmxUtil.tagHandler.logvol.zeroId(elem);
    }

    gmxUtil.tagHandler.logvol.process(elem, gmxUtil, toAdd);

    return;
}
