/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//    Process assemblyref items: basically, just find the referenced assembly and call its processor.
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GeoModelXml/AssemblyrefProcessor.h"
#include <string>
#include <xercesc/dom/DOM.hpp>
#include "GeoModelXml/GmxUtil.h"
#include "GeoModelXml/GeoNodeList.h"
#include "GeoModelXml/translate.h"

using namespace std;
using namespace xercesc;

void AssemblyrefProcessor::process(const DOMElement *element, GmxUtil &gmxUtil, GeoNodeList &toAdd) {
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
    if (nodeName != string("assembly")) {
        ServiceHandle<IMessageSvc> msgh("MessageSvc", "GeoModelXml");
        MsgStream log(&(*msgh), "GeoModelXml");
        log << MSG::FATAL << "Error in xml/gmx file: assemblyref " << translate(idref) << " referenced a " << 
                              nodeName << " instead of an assembly.\n";
        exit(999); // Should do better...
    }
//
//    Process it
//
    const XMLCh *zeroid = element->getAttribute(translate("zeroid"));
    if (XMLString::equals(zeroid, translate("true"))) {
        gmxUtil.tagHandler.assembly.zeroId(elem);
    }
    gmxUtil.tagHandler.assembly.process(elem, gmxUtil, toAdd);
    return;
}
