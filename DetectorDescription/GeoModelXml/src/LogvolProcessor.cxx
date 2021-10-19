/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "GeoModelXml/OutputDirector.h"

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
#include "xercesc/util/XMLString.hpp"

// using namespace CLHEP;

using namespace std;
using namespace xercesc;

void LogvolProcessor::process(const DOMElement *element, GmxUtil &gmxUtil, GeoNodeList &toAdd) {
GeoLogVol *lv;
GeoNameTag *physVolName;

    OUTPUT_STREAM;

    gmxUtil.positionIndex.incrementLevel();

    XMLCh * name_tmp = XMLString::transcode("name");
    char *name2release = XMLString::transcode(element->getAttribute(name_tmp));
    string name(name2release);
    XMLString::release(&name2release);
    XMLString::release(&name_tmp);
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
	XMLCh * shape_tmp = XMLString::transcode("shape");
        const XMLCh *shape = element->getAttribute(shape_tmp);
        DOMElement *refShape = doc->getElementById(shape);
        // Check it is a shape... its parent should be a <shapes>. DTD cannot do this for us.
        DOMNode *parent = refShape->getParentNode();
        if (XMLString::compareIString(parent->getNodeName(), XMLString::transcode("shapes")) != 0) {

            char* shape_s = XMLString::transcode (shape);
            msglog << MSG::FATAL << "Processing logvol " << name << 
                    ". Error in gmx file. An IDREF for a logvol shape did not refer to a shape.\n" <<
              "Shape ref was " << shape_s << "; exiting" << endmsg;
            XMLString::release (&shape_s);
            exit (1); // Need to improve...
        }
//
//    What sort of shape?
//
        name2release = XMLString::transcode(refShape->getNodeName());
        string shapeType(name2release);
        XMLString::release(&name2release);
	XMLString::release(&shape_tmp);

        GeoShape *s = (GeoShape *) gmxUtil.geoItemRegistry.find(shapeType)->process(refShape, gmxUtil);
//
//    Get the material
//
	XMLCh * material_tmp = XMLString::transcode("material");
        const XMLCh *material = element->getAttribute(material_tmp);
        DOMElement *refMaterial = doc->getElementById(material);
        // Check it is a material... its parent should be a <materials>. DTD cannot do this for us.
        parent = refMaterial->getParentNode();
	XMLCh * materials_tmp = XMLString::transcode("materials");
        if (XMLString::compareIString(parent->getNodeName(), materials_tmp) != 0) {
            char* material_s = XMLString::transcode (material);
            msglog << MSG::FATAL << "Processing logvol " << name << 
                    ". Error in gmx file. An IDREF for a logvol material did not refer to a material.\n" <<
                    "Material ref was " << material_s << "; exiting" << endmsg;
           XMLString::release (&material_s);
           exit (1); // Need to improve...
        }

        GeoMaterial *m = (GeoMaterial *) gmxUtil.tagHandler.material.process(refMaterial, gmxUtil);

//
//    Make the LogVol and add it to the map ready for next time
//
        lv = new GeoLogVol(name, s, m);
        store->logVol = lv;

	XMLString::release(&material_tmp);
	XMLString::release(&materials_tmp);
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
            name2release = XMLString::transcode(el->getNodeName());
            string name(name2release);
            XMLString::release(&name2release);
            gmxUtil.processorRegistry.find(name)->process(el, gmxUtil, childrenAdd);
        }
    }
//
//   Make a list of things to be added
//
    toAdd.push_back(physVolName);
    XMLCh * sensitive_tmp = XMLString::transcode("sensitive"); 
    bool sensitive = element->hasAttribute(sensitive_tmp);
    int sensId = 0;
    //std::vector<int> extraSensIds;//extra Identfiers to be used in case we split this volume into multiple DetectorElements
    map<string, int> index;
    //map<string, int> updatedIndex;//extra indices to be used in case we split this volume
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
    XMLString::release(&sensitive_tmp);
    //
    //    Make a new PhysVol and add everything to it, then add it to the list of things for my caller to add
    //
    XMLCh * alignable_tmp = XMLString::transcode("alignable");
    char *toRelease = XMLString::transcode(element->getAttribute(alignable_tmp));
    string alignable(toRelease);
    XMLString::release(&toRelease);
    XMLString::release(&alignable_tmp);
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
	XMLCh * sensitive_tmp = XMLString::transcode("sensitive");
	name2release = XMLString::transcode(element->getAttribute(sensitive_tmp));
	string sensitiveName(name2release);
	XMLString::release(&name2release);
	XMLString::release(&sensitive_tmp);
	//splitting sensors where we would like multiple DetectorElements per GeoVFullPhysVol (e.g.ITk Strips)
	XMLCh * splitLevel_tmp = XMLString::transcode("splitLevel");
	bool split = element->hasAttribute(splitLevel_tmp);
	char* splitString;
	int splitLevel = 1;
	if (split) {
	  splitString = XMLString::transcode(element->getAttribute(splitLevel_tmp));
	  splitLevel = gmxUtil.evaluate(splitString);
	  XMLString::release(&splitString);
	  XMLString::release(&splitLevel_tmp);
	  for(int i=0;i<splitLevel;i++){
	    std::string field = "eta_module";//eventually specify in Xml the field to split in?
	    std::pair<std::string,int> extraIndex(field,i);
	    gmxUtil.gmxInterface()->addSplitSensor(sensitiveName, index,extraIndex, sensId, dynamic_cast<GeoVFullPhysVol *> (pv));
	  }
	}
	else gmxUtil.gmxInterface()->addSensor(sensitiveName, index, sensId, dynamic_cast<GeoVFullPhysVol *> (pv));
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
  
  XMLCh * name_tmp = XMLString::transcode("name");
  char *name2release = XMLString::transcode(element->getAttribute(name_tmp));
  string name(name2release);
  XMLString::release(&name2release);
  XMLString::release(&name_tmp);
  //
  //    Look for the logvol in the map; if not yet there, add it
  //
  map<string, LogVolStore>::iterator entry;
  if ((entry = m_map.find(name)) != m_map.end()) {
    entry->second.id = 0;
  }
  /* else: Not an error: it is usually just about to be made with id = 0; no action needed. */
}
