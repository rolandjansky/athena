/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//    Process logvolref items: basically, just find the referenced logvol and call its processor.
//
#include "GeoModelXml/LogvolrefProcessor.h"
#include <iostream>
#include <string>
#include <xercesc/dom/DOM.hpp>
#include "GeoModelXml/GeoNodeList.h"
#include "GeoModelXml/translate.h"
#include "GeoModelXml/GmxUtil.h"

using namespace std;
using namespace xercesc;

void LogvolrefProcessor::process(const DOMElement *element, GmxUtil &gmxUtil, GeoNodeList &toAdd) {
const XMLCh *ref = translate("ref");
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
    toRelease = translate(elem->getNodeName());
    string nodeName(toRelease);
    XMLString::release(&toRelease);
    if (nodeName != string("logvol")) {
        cerr << "Error in xml/gmx file: logvolref " << translate(idref) << " referenced a " << nodeName << 
                " instead of a logvol.\n";
        exit(999); // Should do better...
    }
//
//    Process it
//
    const XMLCh *zeroid = element->getAttribute(translate("zeroid"));
    if (XMLString::equals(zeroid, translate("true"))) {
        gmxUtil.tagHandler.logvol.zeroId(elem);
    }

    gmxUtil.tagHandler.logvol.process(elem, gmxUtil, toAdd);

    return;
}
