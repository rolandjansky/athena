/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STANDALONE_GMX
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#else
#include <iostream>
#endif

#include "GeoModelXml/Element2GeoItem.h"
#include <string>
#include <xercesc/dom/DOM.hpp>
#include "GeoModelXml/GmxUtil.h"
#include "GeoModelKernel/RCBase.h"
#include "GeoModelXml/translate.h"

using namespace std;
using namespace xercesc;

Element2GeoItem::Element2GeoItem() {}

Element2GeoItem::~Element2GeoItem() {}

const RCBase * Element2GeoItem::process(const xercesc::DOMElement *element, GmxUtil &gmxUtil) {

    char *name2release;

    name2release = Translate(element->getAttribute(Translate("name")));
    string name(name2release);
    XMLString::release(&name2release);

    const RCBase *item;
    map<string, const RCBase *>::iterator entry;
    if (name == "") { // Unnamed item; cannot store in the map; make a new one 
        item = make(element, gmxUtil);
    }
    else if ((entry = m_map.find(name)) == m_map.end()) { // Not in; make a new one
        item = make(element, gmxUtil);
        m_map[name] = item; // And put it in the map
    }
    else { // Get it from the map
        item = entry->second; 
    }

    return item;
}

const RCBase * Element2GeoItem::make(const xercesc::DOMElement *element, GmxUtil & /* gmxUtil */) const {
    char *name2release = Translate(element->getNodeName());

#ifndef STANDALONE_GMX
    ServiceHandle<IMessageSvc> msgh("MessageSvc", "GeoModelXml");
    MsgStream log(&(*msgh), "GeoModelXml");
    log << MSG::FATAL << "Oh oh: called base class make() method of Element2GeoType object; tag " << name2release << endmsg;
#else
    std::cerr<< "Oh oh: called base class make() method of Element2GeoType object; tag " << name2release << std::endl;
#endif
    XMLString::release(&name2release);

    exit(999); // Should improve on this 
}
