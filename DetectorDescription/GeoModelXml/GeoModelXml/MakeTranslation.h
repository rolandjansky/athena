/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO_MODEL_XML_MAKE_TRANSLATION_H
#define GEO_MODEL_XML_MAKE_TRANSLATION_H
#include <xercesc/util/XercesDefs.hpp>
//
//    Create and return a HepRotation3d. Caller must delete it.
//

XERCES_CPP_NAMESPACE_BEGIN
class DOMElement;
XERCES_CPP_NAMESPACE_END
#include "GeoModelKernel/GeoDefinitions.h"
class GmxUtil;


class MakeTranslation {
public:
    MakeTranslation();
    GeoTrf::Translate3D getTransform(const xercesc::DOMElement *rotation, GmxUtil &gmxUtil);
};
#endif // MAKE_TRANSLATION_H
