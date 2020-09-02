/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/shape/MakeUnion.h"
#include <string>

#ifndef STANDALONE_GMX
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#else
#include <iostream>
#endif

#include <xercesc/dom/DOM.hpp>
//   #include <CLHEP/Geometry/Transform3D.h>
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoDefinitions.h"

#include "GeoModelXml/translate.h"
#include "GeoModelXml/GmxUtil.h"

using namespace xercesc;
using namespace std;

MakeUnion::MakeUnion() {}

const RCBase * MakeUnion::make(const xercesc::DOMElement *element, GmxUtil &gmxUtil) const {
// 
//    Process child elements; first is first shaperef; then transform; then second shaperef.
//
    GeoShape *first = 0;
    GeoShape *second = 0;
    GeoTrf::Transform3D hepXf=GeoTrf::Transform3D::Identity();
    int elementIndex = 0;
    for (DOMNode *child = element->getFirstChild(); child != 0; child = child->getNextSibling()) {
        if (child->getNodeType() == DOMNode::ELEMENT_NODE) { // Skips text nodes
            switch (elementIndex) {
                case 0: { // First element is first shaperef
                    first = (GeoShape *) gmxUtil.tagHandler.shaperef.process(dynamic_cast<DOMElement *> (child), gmxUtil);
                    break;
                }
                case 1: { // Second element is transformation or transformationref
                    char *toRelease = Translate(child->getNodeName());
                    string nodeName(toRelease);
                    XMLString::release(&toRelease);
                    GeoTransform *geoXf = nodeName == "transformation"? 
(GeoTransform *) gmxUtil.tagHandler.transformation.process(dynamic_cast<DOMElement *>(child), gmxUtil):
(GeoTransform *) gmxUtil.tagHandler.transformationref.process(dynamic_cast<DOMElement *>(child), gmxUtil);
                    hepXf = geoXf->getTransform();
                    break;
                }
                case 2: { // Third element is second shaperef
                    second = (GeoShape *) gmxUtil.tagHandler.shaperef.process(dynamic_cast<DOMElement *> (child), gmxUtil);
                    break;
                }
                default: // More than 3 elements?
#ifndef STANDALONE_GMX
                    ServiceHandle<IMessageSvc> msgh("MessageSvc", "GeoModelXml");
                    MsgStream log(&(*msgh), "GeoModelXml");
                    log << MSG::FATAL << "MakeUnion: Incompatible DTD? got more than 3 child elements" << endmsg;
#else
		    std::cout<<"MakeUnion: Incompatible DTD? got more than 3 child elements" << std::endl;
#endif
            }
            elementIndex++;
        }
    }

    const GeoShapeUnion *temp = &(first->add(*(GeoShape *) &(*(second) << hepXf)));

    return (RCBase *) temp;
}
