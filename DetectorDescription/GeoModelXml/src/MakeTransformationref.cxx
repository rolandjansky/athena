/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//    Process transformationref items: basically, just find the referenced transform and call its processor.
//
#include "GeoModelXml/MakeTransformationref.h"

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

const RCBase *MakeTransformationref::make(const DOMElement *element, GmxUtil &gmxUtil) const {
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
    if (nodeName != string("transformation")) {
#ifndef STANDALONE_GMX
        ServiceHandle<IMessageSvc> msgh("MessageSvc", "GeoModelXml");
        MsgStream log(&(*msgh), "GeoModelXml");
        log << MSG::FATAL << "Error in xml/gmx file: transformationref " << Translate(idref) << " referenced a " << nodeName << 
                " instead of a transformation." << endmsg;
#else
	std::cout<<"Error in xml/gmx file: transformationref " << Translate(idref) << " referenced a " << nodeName << 
                " instead of a transformation." <<std::endl;
#endif
        exit(999); // Should do better...
    }
//
//    Process it
//
    return gmxUtil.tagHandler.transformation.process(elem, gmxUtil);
}
