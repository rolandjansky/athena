/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//
//    Handle material elements.
//
#ifndef GEO_MODEL_XML_MAKE_MATERIAL_H
#define GEO_MODEL_XML_MAKE_MATERIAL_H
#include <xercesc/util/XercesDefs.hpp>

#include "GeoModelXml/Element2GeoItem.h"

class MakeMaterial: public Element2GeoItem {
public:
    MakeMaterial();
    virtual RCBase * make(const xercesc::DOMElement *element, GmxUtil &gmxUtil) const override;
};

#endif // MAKE_MATERIAL_H
