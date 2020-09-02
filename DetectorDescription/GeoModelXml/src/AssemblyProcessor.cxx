/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//   Processor for assembly elements
//
//   Add name to list.
//   Create a physvol using special logvol GmxUtil.getAssemblyLV() add it to list.
//      ...by using material "special::ether" Geo2G4 turns this into a G4Assembly
//   Process children and get list of things to be added to the physvol.
//   Add them to the physvol.
//
#include "GeoModelXml/AssemblyProcessor.h"

#include <map>

#include <xercesc/dom/DOM.hpp>
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelXml/GmxUtil.h"
#include "GeoModelXml/GeoNodeList.h"
#include "GeoModelXml/translate.h"

// using namespace CLHEP;

using namespace std;
using namespace xercesc;

void AssemblyProcessor::process(const DOMElement *element, GmxUtil &gmxUtil, GeoNodeList &toAdd) {
GeoLogVol *lv;
GeoNameTag *physVolName;

    gmxUtil.positionIndex.incrementLevel();

    char *name2release = Translate(element->getAttribute(Translate("name")));
    string name(name2release);
    XMLString::release(&name2release);
//
//    Look for the assembly in the map; if not yet there, add it
//
    map<string, AssemblyStore>::iterator entry;
    if ((entry = m_map.find(name)) == m_map.end()) { // Not in registry; make a new item
//
//    Name
//
        m_map[name] = AssemblyStore();
        AssemblyStore *store = &m_map[name];
        physVolName = new GeoNameTag(name); 
        store->name = physVolName;
        store->id = 0;
    }
    else { // Already in the registry; use it.
        physVolName = entry->second.name;
    }
    lv = gmxUtil.getAssemblyLV();
    toAdd.push_back(physVolName);
    gmxUtil.positionIndex.setCopyNo(m_map[name].id);
    toAdd.push_back(new GeoIdentifierTag(m_map[name].id++)); 
//
//    Process the assembly's children
//
    GeoNodeList childrenAdd;
    for (DOMNode *child = element->getFirstChild(); child != 0; child = child->getNextSibling()) {
        if (child->getNodeType() == DOMNode::ELEMENT_NODE) {
            DOMElement *el = dynamic_cast<DOMElement *> (child);
            name2release = Translate(el->getNodeName());
            string name(name2release);
            XMLString::release(&name2release);
            gmxUtil.processorRegistry.find(name)->process(el, gmxUtil, childrenAdd);
        }
    }
//
//    Make a new PhysVol and add everything to it, then add it to the list of things for my caller to add
//
    char *toRelease = Translate(element->getAttribute(Translate("alignable")));
    string alignable(toRelease);
    XMLString::release(&toRelease);
    if (alignable.compare(string("true")) == 0) {
        GeoFullPhysVol *pv = new GeoFullPhysVol(lv);
        for (GeoNodeList::iterator node = childrenAdd.begin(); node != childrenAdd.end(); ++node) {
            pv->add(*node);
        }
        toAdd.push_back(pv); // NB: the *PV is third item added, so reference as toAdd[2].
    }
    else {
        GeoPhysVol *pv = new GeoPhysVol(lv);
        for (GeoNodeList::iterator node = childrenAdd.begin(); node != childrenAdd.end(); ++node) {
            pv->add(*node);
        }
        toAdd.push_back(pv);
    }

    gmxUtil.positionIndex.decrementLevel();

    return;
}

void AssemblyProcessor::zeroId(const xercesc::DOMElement *element) {

    char *name2release = Translate(element->getAttribute(Translate("name")));
    string name(name2release);
    XMLString::release(&name2release);
//
//    Look for the assembly in the map; if not yet there, add it
//
    map<string, AssemblyStore>::iterator entry;
    if ((entry = m_map.find(name)) != m_map.end()) {
        entry->second.id = 0;
    }
/* else ... Not an error: it is usually just about to be made with id = 0; no action needed. */
}
