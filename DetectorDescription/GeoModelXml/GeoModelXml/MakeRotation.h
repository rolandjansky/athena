/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO_MODEL_XML_MAKE_ROTATION_H
#define GEO_MODEL_XML_MAKE_ROTATION_H
//
//    Create and return a HepRotation3d. Caller must delete it.
//

#include <xercesc/dom/DOM.hpp>
#include "GeoModelKernel/GeoDefinitions.h"

//namespace HepGeom {class Rotate3D;}
class GmxUtil;


class MakeRotation {
public:
    MakeRotation();
    GeoTrf::Rotation3D getTransform(const xercesc::DOMElement *rotation, GmxUtil &gmxUtil);
};
#endif // MAKE_ROTATION_H
