/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//   multicopy element processor.
//
//   First time round, a loop creates all the Geo-transforms needed and stores them. First and subsequent calls, 
//   a second loop adds these to the toAdd list, along with copies of the object. The object-handler stores and 
//   deals with the object. This si needed to make sure sensitive volumes and alignable volumes are always added 
//   correctly. 
//
//   The copy number of the copied object can be zeroed each time the multicopy-element is processed if the 
//   zeroid attribute is set true. This has to be done here - if the attribute is given on a logvol or assembly ref, 
//   it would alwasy be zero.
//
//   There are two ways of making the n transformations:
//        loopvar attribute not set:    The transformation is raised to the power i-1
//        loopvar set to a vector name: In the loop, before each transformation is created, the vector generic name is set
//                                      equal to the next element of the vector.
//
#include "GeoModelXml/MulticopyProcessor.h"

#ifndef STANDALONE_GMX
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#else
#include <iostream>
#endif

#include <sstream>
#include <string>
#include <cstdlib>
#include <xercesc/dom/DOM.hpp>
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoVFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoDefinitions.h"
//  #include <CLHEP/Geometry/Transform3D.h>

#include "GeoModelXml/translate.h"
#include "GeoModelXml/GeoNodeList.h"
#include "GeoModelXml/ProcessorRegistry.h"
#include "GeoModelXml/GmxUtil.h"


using namespace xercesc;
using namespace std;

void MulticopyProcessor::process(const DOMElement *element, GmxUtil &gmxUtil, GeoNodeList &toAdd) {
char *toRelease;
const XMLCh *ref = Translate("ref");
const XMLCh *idref;
DOMDocument *doc = element->getOwnerDocument();

#ifndef STANDALONE_GMX
    ServiceHandle<IMessageSvc> msgh("MessageSvc", "GeoModelXml");
    MsgStream log(&(*msgh), "GeoModelXml");
#endif

    bool alignable = element->hasAttribute(Translate("alignable"));
//
//    How many copies?
//
    int nCopies;
    toRelease = Translate(element->getAttribute(Translate("n")));
    nCopies = gmxUtil.evaluate(toRelease);
    XMLString::release(&toRelease);
//
//    See if it is in the map; if so, xfList is already done. If not, fill xfList.
//
    toRelease = Translate(element->getAttribute(Translate("name")));
    string name(toRelease);
    XMLString::release(&toRelease);
    map<string, GeoNodeList>::iterator entry;
    GeoNodeList *xfList;
    if ((entry = m_map.find(name)) == m_map.end()) { // Not in registry; make a new item
//
//    Add empty node list to the map
//
        m_map[name] = GeoNodeList();
        xfList = &m_map[name];
//
//    Loopvar Variable name
//
        string varname, firstElement;
        bool hasVarname = (element->getAttributeNode(Translate("loopvar")) != 0);
        if (hasVarname) {
            toRelease = Translate(element->getAttribute(Translate("loopvar")));
            varname = toRelease;
            XMLString::release(&toRelease);
            // Check it is a vector
            firstElement = varname + "_0";
            if (!gmxUtil.eval.findVariable(firstElement.c_str())) {
#ifndef STANDALONE_GMX
                log << MSG::FATAL << "Error in .gmx file. Processing multicopy element with name " << name << 
                ". Found loopvar set to " << varname << ", but no vector with that name has been defined." << endmsg;
#else
		std::cout<< "Error in .gmx file. Processing multicopy element with name " << name << 
                ". Found loopvar set to " << varname << ", but no vector with that name has been defined." << std::endl;
#endif

                exit(999); // Should do better
            }
        }
//
//    Get the transformation-element
//
        DOMElement *elXf = element->getFirstElementChild();
        toRelease = Translate(elXf->getNodeName());
        string nodeName(toRelease);
        XMLString::release(&toRelease);
        Element2GeoItem *xFormProcessor;
        if (hasVarname) {
            if (nodeName == "transformation") { // OK
                xFormProcessor = (Element2GeoItem *) &(gmxUtil.tagHandler.transformation);
            }
            else { // Not OK
#ifndef STANDALONE_GMX
                log << MSG::FATAL << "Error in .gmx file. Processing multicopy element with name " << name <<
                ". \nIt gives loopvar therefore should have a <transformation> and not a <transformationref> (despite the DTD)\n";
#else
		std::cout<<"Error in .gmx file. Processing multicopy element with name " << name <<
                ". \nIt gives loopvar therefore should have a <transformation> and not a <transformationref> (despite the DTD)\n";
#endif
                exit(999); // Should do better
            }
        }
        else {
            xFormProcessor = nodeName == "transformation"?
                             (Element2GeoItem *) &(gmxUtil.tagHandler.transformation):
                             (Element2GeoItem *) &(gmxUtil.tagHandler.transformationref);
        }
//
//    Produce all the transformations
//
        GeoAlignableTransform *geoAXf;
        GeoTransform *geoXf;
        if (hasVarname) {
            for (int i = 0; i < nCopies; ++i) {
                gmxUtil.eval.setVariable(varname.c_str(), (varname + "_" + to_string(i)).c_str());
                if (alignable) {
                    geoAXf = (GeoAlignableTransform *) xFormProcessor->make(elXf, gmxUtil);
                    xfList->push_back((GeoGraphNode *) geoAXf);
                }
                else {
                    geoXf = (GeoTransform *) xFormProcessor->make(elXf, gmxUtil);
                    xfList->push_back((GeoGraphNode *) geoXf);
                }
                gmxUtil.eval.removeVariable(varname.c_str()); // Avoids a warning status in evaluator
            }
        }
        else {
//
//    If varname not given, we get the CLHEP xForm and raise it to the power i, so NOT applied to first object.
//    No transform (i.e. identity) for the first; so one less transform than objects
//
	  GeoTrf::Transform3D hepXf0=GeoTrf::Transform3D::Identity();
            if (alignable) {
                geoAXf = (GeoAlignableTransform *) xFormProcessor->make(elXf, gmxUtil);
                hepXf0 = geoAXf->getTransform();
            }
            else {
                geoXf = (GeoTransform *) xFormProcessor->make(elXf, gmxUtil);
                hepXf0 = geoXf->getTransform();
            }
            GeoTrf::Transform3D hepXf=GeoTrf::Transform3D::Identity(); // Identity initially
            for (int i = 0; i < nCopies; ++i) {
                xfList->push_back((GeoGraphNode *) new GeoTransform(hepXf));
                hepXf = hepXf0 * hepXf;
            }
        }
    }
    else {
        xfList = &entry->second;
    }
//
//    Get object to be copied
//
    DOMElement *object = element->getLastElementChild();
    toRelease = Translate(object->getNodeName());
    string nodeName(toRelease);
    XMLString::release(&toRelease);
    ElementProcessor *objectProcessor = gmxUtil.processorRegistry.find(nodeName);
//
//    Zero its copy number. Only needed if an item is used in 2 or more multicopies;
//    harmless in case of only one use.
//
    if (nodeName == "logvolref") {
        idref = object->getAttribute(ref);
        DOMElement *elem = doc->getElementById(idref);
        gmxUtil.tagHandler.logvol.zeroId(elem);
    }
    else if (nodeName == "assemblyref") {
        idref = object->getAttribute(ref);
        DOMElement *elem = doc->getElementById(idref);
        gmxUtil.tagHandler.assembly.zeroId(elem);
    }
    else if (nodeName == "transform") { // Object is either an assemlyref or logvolref, and there is only one of these
        DOMNodeList *lvList = element->getElementsByTagName(Translate("logvolref"));
        if (lvList->getLength() > 0) {
            const XMLCh *idref =  dynamic_cast<DOMElement *> (lvList->item(0))->getAttribute(Translate("ref"));
            DOMElement *lv = doc->getElementById(idref);
            gmxUtil.tagHandler.logvol.zeroId(lv);
        }
        else {
            DOMNodeList *asList = element->getElementsByTagName(Translate("assemblyref"));
            if (asList->getLength() > 0) {
                const XMLCh *idref =  dynamic_cast<DOMElement *> (asList->item(0))->getAttribute(Translate("ref"));
                DOMDocument *doc = element->getOwnerDocument();
                DOMElement *as = doc->getElementById(idref);
                gmxUtil.tagHandler.assembly.zeroId(as);
            }
            else {
#ifndef STANDALONE_GMX
                log << MSG::FATAL << 
                    "multicopyprocessor: error in " << name << ". <transform> object was neither assemblyref nor logvolref\n"
                    << "Exiting Athena" << endmsg;
#else
		std::cout<<"multicopyprocessor: error in " << name << ". <transform> object was neither assemblyref nor logvolref\n"
                    << "Exiting Athena" << std::endl;
#endif
                exit(999); // Should do better
            }
        }
    }
//
//    If alignable, add transformation to DetectorManager via GmxInterface.
//    (NB. Alignable is messy because it involves both a transformation and an object as well as
//    the multicopy or transform element to tell us when to insert it and what level of alignment 
//    is wanted; and passing info from one routine to another when you try to keep a uniform interface is 
//    difficult; and we only have partial GeoModel trees so we cannot use GeoModel tree traversal at this 
//    moment (comes back to no way of knowing if we have a GeoFullPV or GeoPV)).
//
//    Also: no one is using it at the time of writing, so it is ***TOTALLY*** untested.
//    It is done now to (i) make sure there ought to be a solution (ii) implement (imperfectly) 
//    a way now while things are fresh in my mind.
//
    int level;
    if (alignable) {
        istringstream(Translate(element->getAttribute(Translate("alignable")))) >> level;
    }
//
//    Add transforms and physvols etc. to list to be added
//
    map<string, int> index;
    for (int copy = 0; copy < nCopies; ++copy) {
        toAdd.push_back((*xfList)[copy]);
        int lastTransform = toAdd.size() - 1;
        objectProcessor->process(object, gmxUtil, toAdd);
        if (alignable) {

            cout << "copy = " << copy << "; level = " << level << endl;
            cout << "\nAdd Alignable named " << endl;
            cout << ((GeoNameTag *) (toAdd[lastTransform + 1]))->getName() << endl;
            cout << " with id " << endl;
            cout << ((GeoIdentifierTag *) (toAdd[lastTransform + 2]))->getIdentifier() << endl;

            gmxUtil.positionIndex.incrementLevel(); // Logvol has unfortunately already decremented this; temp. restore it
            gmxUtil.positionIndex.indices(index, gmxUtil.eval);
            gmxUtil.gmxInterface()->addAlignable(level, index, (GeoVFullPhysVol *) toAdd[lastTransform + 3],
                                                 (GeoAlignableTransform *) toAdd[lastTransform]);
            gmxUtil.positionIndex.decrementLevel(); // Put it back where it was
            index.clear();
        }
    }
}
