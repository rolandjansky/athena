/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//    Process transformref items: basically, just find the referenced transform and call its processor.
//
#include "GeoModelXml/TransformrefProcessor.h"

#ifndef STANDALONE_GMX
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#else
#include <iostream>
#endif

#include <string>
#include <xercesc/dom/DOM.hpp>
#include "GeoModelXml/GmxUtil.h"
#include "GeoModelXml/GeoNodeList.h"
#include "GeoModelXml/translate.h"

using namespace std;
using namespace xercesc;

void TransformrefProcessor::process(const DOMElement *element, GmxUtil &gmxUtil, GeoNodeList &toAdd) {
const XMLCh *ref = Translate("ref");
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
    toRelease = Translate(elem->getNodeName());
    string nodeName(toRelease);
    XMLString::release(&toRelease);
    if (nodeName != "transform") {
#ifndef STANDALONE_GMX
        ServiceHandle<IMessageSvc> msgh("MessageSvc", "GeoModelXml");
        MsgStream log(&(*msgh), "GeoModelXml");
        log << MSG::FATAL<< "Error in xml/gmx file: transformref " << Translate(idref) << " referenced a " << nodeName << 
                " instead of a logvol." << endmsg;
#else
	std::cout <<"Error in xml/gmx file: transformref " << Translate(idref) << " referenced a " << nodeName << 
                " instead of a logvol." << std::endl;
#endif
        exit(999); // Should do better...
    }
//
//    Process it
//
    gmxUtil.tagHandler.transform.process(elem, gmxUtil, toAdd);

    return;
}
