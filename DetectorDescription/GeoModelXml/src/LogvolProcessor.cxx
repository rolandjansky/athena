/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//   Processor for logvol elements
//
//   Add name to list.
//   Create a physvol using my logvol; add it to list.
//   Process children and get list of things to be added to the physvol.
//   Add them to the physvol.
//
#include "GeoModelXml/LogvolProcessor.h"

#ifndef STANDALONE_GMX
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#else
#include <iostream>
#endif

#include <map>

#include <xercesc/dom/DOM.hpp>
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoVFullPhysVol.h"
#include "GeoModelXml/GmxUtil.h"
#include "GeoModelXml/GeoNodeList.h"
#include "GeoModelXml/translate.h"

// using namespace CLHEP;

using namespace std;
using namespace xercesc;

void LogvolProcessor::process(const DOMElement *element, GmxUtil &gmxUtil, GeoNodeList &toAdd) {
GeoLogVol *lv;
GeoNameTag *physVolName;

#ifndef STANDALONE_GMX
    ServiceHandle<IMessageSvc> msgh("MessageSvc", "GeoModelXml");
    MsgStream log(&(*msgh), "GeoModelXml");
#endif

    gmxUtil.positionIndex.incrementLevel();

    char *name2release = Translate(element->getAttribute(Translate("name")));
    string name(name2release);
    XMLString::release(&name2release);
//
//    Look for the logvol in the map; if not yet there, add it
//
    map<string, LogVolStore>::iterator entry;
    if ((entry = m_map.find(name)) == m_map.end()) { // Not in registry; make a new item
//
//    Name
//
        m_map[name] = LogVolStore();
        LogVolStore *store = &m_map[name];
        physVolName = new GeoNameTag(name); 
        store->name = physVolName;
        store->id = 0;
//
//    Get the shape.
//
        DOMDocument *doc = element->getOwnerDocument();
        const XMLCh *shape = element->getAttribute(Translate("shape"));
        DOMElement *refShape = doc->getElementById(shape);
        // Check it is a shape... its parent should be a <shapes>. DTD cannot do this for us.
        DOMNode *parent = refShape->getParentNode();
        if (XMLString::compareIString(parent->getNodeName(), Translate("shapes")) != 0) {

#ifndef STANDALONE_GMX
            log << MSG::FATAL << "Processing logvol " << name << 
                    ". Error in gmx file. An IDREF for a logvol shape did not refer to a shape.\n" <<
                    "Shape ref was " << shape << "; exiting" << endmsg;
#else
	    std::cout<<"Processing logvol " << name << 
                    ". Error in gmx file. An IDREF for a logvol shape did not refer to a shape.\n" <<
                    "Shape ref was " << shape << "; exiting" <<std::endl;
#endif
            exit (1); // Need to improve...
        }
//
//    What sort of shape?
//
        name2release = Translate(refShape->getNodeName());
        string shapeType(name2release);
        XMLString::release(&name2release);

        GeoShape *s = (GeoShape *) gmxUtil.geoItemRegistry.find(shapeType)->process(refShape, gmxUtil);
//
//    Get the material
//
        const XMLCh *material = element->getAttribute(Translate("material"));
        DOMElement *refMaterial = doc->getElementById(material);
        // Check it is a material... its parent should be a <materials>. DTD cannot do this for us.
        parent = refMaterial->getParentNode();
        if (XMLString::compareIString(parent->getNodeName(), Translate("materials")) != 0) {
#ifndef STANDALONE_GMX
            log << MSG::FATAL << "Processing logvol " << name << 
                    ". Error in gmx file. An IDREF for a logvol material did not refer to a material.\n" <<
                    "Material ref was " << material << "; exiting" << endmsg;
#else
	    std::cout<<"Processing logvol " << name << 
                    ". Error in gmx file. An IDREF for a logvol material did not refer to a material.\n" <<
                    "Material ref was " << material << "; exiting" <<std::endl;
#endif
            exit (1); // Need to improve...
        }

        GeoMaterial *m = (GeoMaterial *) gmxUtil.tagHandler.material.process(refMaterial, gmxUtil);
//
//    Make the LogVol and add it to the map ready for next time
//
        lv = new GeoLogVol(name, s, m);
        store->logVol = lv;
    }
    else { // Already in the registry; use it.
        physVolName = entry->second.name;
        lv = entry->second.logVol;
    }
//
//    Process the logvol children (side effect: sets formulae for indexes before calculating them)
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
//   Make a list of things to be added
//
    toAdd.push_back(physVolName);
    bool sensitive = element->hasAttribute(Translate("sensitive"));
    int sensId = 0;
    map<string, int> index;
    if (sensitive) {
        gmxUtil.positionIndex.setCopyNo(m_map[name].id++);
        gmxUtil.positionIndex.indices(index, gmxUtil.eval);
        sensId = gmxUtil.gmxInterface()->sensorId(index);
//        toAdd.push_back(new GeoIdentifierTag(m_map[name].id)); // Normal copy number
        toAdd.push_back(new GeoIdentifierTag(sensId)); 
    }
    else {
        toAdd.push_back(new GeoIdentifierTag(m_map[name].id)); // Normal copy number
        gmxUtil.positionIndex.setCopyNo(m_map[name].id++);
    } 
//
//    Make a new PhysVol and add everything to it, then add it to the list of things for my caller to add
//
    char *toRelease = Translate(element->getAttribute(Translate("alignable")));
    string alignable(toRelease);
    XMLString::release(&toRelease);
    if (sensitive || (alignable.compare(string("true")) == 0)) {
        GeoFullPhysVol *pv = new GeoFullPhysVol(lv);
        for (GeoNodeList::iterator node = childrenAdd.begin(); node != childrenAdd.end(); ++node) {
            pv->add(*node);
        }
        toAdd.push_back(pv); // NB: the *PV is third item added, so reference as toAdd[2].
//
//    Add sensitive volumes to detector manager via GmxInterface
//
        if (sensitive) {
            name2release = Translate(element->getAttribute(Translate("sensitive")));
            string sensitiveName(name2release);
            XMLString::release(&name2release);
            gmxUtil.gmxInterface()->addSensor(sensitiveName, index, sensId, dynamic_cast<GeoVFullPhysVol *> (pv));
        }
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

void LogvolProcessor::zeroId(const xercesc::DOMElement *element) {

    char *name2release = Translate(element->getAttribute(Translate("name")));
    string name(name2release);
    XMLString::release(&name2release);
//
//    Look for the logvol in the map; if not yet there, add it
//
    map<string, LogVolStore>::iterator entry;
    if ((entry = m_map.find(name)) != m_map.end()) {
        entry->second.id = 0;
    }
/* else: Not an error: it is usually just about to be made with id = 0; no action needed. */
}
