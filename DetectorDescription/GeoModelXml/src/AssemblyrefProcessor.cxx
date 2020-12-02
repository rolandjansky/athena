/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//    Process assemblyref items: basically, just find the referenced assembly and call its processor.
//
#include "GeoModelXml/OutputDirector.h"
#include "GeoModelXml/AssemblyrefProcessor.h"
#include <string>

#include "xercesc/util/XercesDefs.hpp"
#include <xercesc/dom/DOM.hpp>
#include "xercesc/util/XMLString.hpp"

#include "GeoModelXml/GmxUtil.h"
#include "GeoModelXml/GeoNodeList.h"

using namespace std;
using namespace xercesc;

void AssemblyrefProcessor::process(const DOMElement *element, GmxUtil &gmxUtil, GeoNodeList &toAdd) {
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
    if (nodeName != string("assembly")) {
        OUTPUT_STREAM;
        msglog << MSG::FATAL << "Error in xml/gmx file: assemblyref " << XMLString::transcode(idref) << " referenced a " << 
                              nodeName << " instead of an assembly.\n";
        exit(999); // Should do better...
    }
//
//    Process it
//
    const XMLCh *zeroid = element->getAttribute(XMLString::transcode("zeroid"));
    if (XMLString::equals(zeroid, XMLString::transcode("true"))) {
        gmxUtil.tagHandler.assembly.zeroId(elem);
    }
    gmxUtil.tagHandler.assembly.process(elem, gmxUtil, toAdd);
    return;
}
