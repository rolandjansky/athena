/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO_MODEL_XML_MAKE_TRANSLATION_H
#define GEO_MODEL_XML_MAKE_TRANSLATION_H
//
//    Create and return a HepRotation3d. Caller must delete it.
//

#include "GeoModelKernel/GeoDefinitions.h"

#include <xercesc/dom/DOM.hpp>

//namespace HepGeom {class Translate3D;}
class GmxUtil;


class MakeTranslation {
public:
    MakeTranslation();
    GeoTrf::Translate3D getTransform(const xercesc::DOMElement *rotation, GmxUtil &gmxUtil);
};
#endif // MAKE_TRANSLATION_H
