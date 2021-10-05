/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Automatically generated code from /home/hessey/prog/gmx2geo/makeshape
// Then heavily modified
#include "GeoModelXml/shape/MakeIntersection.h"
#include <string>
#include "GeoModelXml/OutputDirector.h"
#include <xercesc/dom/DOM.hpp>
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoTransform.h"

#include "xercesc/util/XMLString.hpp"
#include "GeoModelXml/GmxUtil.h"
#include "CxxUtils/checker_macros.h"

using namespace xercesc;
using namespace std;

MakeIntersection::MakeIntersection() {}

RCBase * MakeIntersection::make(const xercesc::DOMElement *element, GmxUtil &gmxUtil) const {
// 
//    Process child elements; first is first shaperef; then transform; then second shaperef.
//
    const GeoShape *first = 0;
    const GeoShape *second = 0;
    GeoTrf::Transform3D hepXf=GeoTrf::Transform3D::Identity(); 
    int elementIndex = 0;
    for (DOMNode *child = element->getFirstChild(); child != 0; child = child->getNextSibling()) {
        if (child->getNodeType() == DOMNode::ELEMENT_NODE) { // Skips text nodes
            switch (elementIndex) {
                case 0: { // First element is first shaperef
                    first = static_cast<const GeoShape *> (gmxUtil.tagHandler.shaperef.process(dynamic_cast<DOMElement *> (child), gmxUtil));
                    break;
                }
                case 1: { // Second element is transformation or transformationref
                    char *toRelease = XMLString::transcode(child->getNodeName());
                    string nodeName(toRelease);
                    XMLString::release(&toRelease);
                    const GeoTransform *geoXf = nodeName == "transformation"? 
static_cast<const GeoTransform *> (gmxUtil.tagHandler.transformation.process(dynamic_cast<DOMElement *>(child), gmxUtil)):
static_cast<const GeoTransform *> (gmxUtil.tagHandler.transformationref.process(dynamic_cast<DOMElement *>(child), gmxUtil));
                    hepXf = geoXf->getTransform();
                    break;
                }
                case 2: { // Third element is second shaperef
                    second = static_cast<const GeoShape *> (gmxUtil.tagHandler.shaperef.process(dynamic_cast<DOMElement *> (child), gmxUtil));
                    break;
                }
                default: // More than 3 elements?
                    OUTPUT_STREAM;
                    msglog << MSG::FATAL << "MakeIntersection: Incompatible DTD? got more than 3 child elements\n";
            }
            elementIndex++;
        }
    }

    // FIXME: intersect() returns a new'd object --- should really be
    // returning a `unique_ptr<GeoShapeIntersection>' not a
    // `const GeoShapeIntersection'
    GeoShapeIntersection *temp ATLAS_THREAD_SAFE = const_cast<GeoShapeIntersection*>(&(first->intersect(*(GeoShape *) &(*(second) << hepXf))));

    return (RCBase *) temp;
}
