/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//   Processor for transform elements
//
//   Decide if I need alignable transforms from my second child
//   Add transform from first child to list
//   Create physvol using my second child; add it to list.
//
#include "GeoModelXml/TransformProcessor.h"

#include <string>
#include <sstream>
#include <vector>
#include <xercesc/dom/DOM.hpp>
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoVFullPhysVol.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelXml/GmxUtil.h"
#include "xercesc/util/XMLString.hpp"


//  using namespace CLHEP;
using namespace std;
using namespace xercesc;

void TransformProcessor::process(const DOMElement *element, GmxUtil &gmxUtil, GeoNodeList &toAdd) {
char *tagName;
 XMLCh * alignable_tmp;

 alignable_tmp = XMLString::transcode("alignable");
 bool alignable = element->hasAttribute(alignable_tmp);
 //
 //    Do second element first, to find what sort of transform is needed (shape or logvol etc.?)
 //
 GeoNodeList objectsToAdd;
 DOMElement *object = element->getLastElementChild();
 tagName = XMLString::transcode(object->getTagName());
 string objectName(tagName);
 gmxUtil.processorRegistry.find(objectName)->process(object, gmxUtil, objectsToAdd);
 XMLString::release(&tagName);
 XMLString::release(&alignable_tmp);
 //
 //    Get the transformation
 //
 DOMElement *transformation = element->getFirstElementChild();
 tagName = XMLString::transcode(transformation->getTagName()); // transformation or transformationref
 //  ******* Should check here that an alignable transform is given an alignable transformation and object; to be done
 toAdd.push_back((GeoGraphNode *)gmxUtil.geoItemRegistry.find(string(tagName))->process(transformation, gmxUtil));
 XMLString::release(&tagName);
 //
 //    Add transformation to DetectorManager via GmxInterface, if it is alignable
//
    if (alignable) { 
        int level;
	alignable_tmp = XMLString::transcode("alignable");
        istringstream(XMLString::transcode(element->getAttribute(alignable_tmp))) >> level;
        map<string, int> index;
        gmxUtil.positionIndex.incrementLevel(); // Logvol has unfortunately already decremented this; temp. restore it
        gmxUtil.positionIndex.indices(index, gmxUtil.eval);
// ****** Using "[2]" is an ugly way to reach the PV. Define const int in LogvolProcessor?
        gmxUtil.gmxInterface()->addAlignable(level, index, (GeoVFullPhysVol *) objectsToAdd[2], 
                                             (GeoAlignableTransform *) toAdd.back());

        gmxUtil.positionIndex.decrementLevel(); 
	XMLString::release(&alignable_tmp);
    }
//
//    And add the name and physvol etc. after the transformation
//
    toAdd.insert(toAdd.end(), objectsToAdd.begin(), objectsToAdd.end());

    return;
}
