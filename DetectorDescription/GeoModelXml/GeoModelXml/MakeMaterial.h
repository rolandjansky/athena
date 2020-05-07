/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//    Handle material elements.
//
#ifndef GEO_MODEL_XML_MAKE_MATERIAL_H
#define GEO_MODEL_XML_MAKE_MATERIAL_H

#include "GeoModelXml/Element2GeoItem.h"

class MakeMaterial: public Element2GeoItem {
public:
    MakeMaterial();
    const RCBase * make(const xercesc::DOMElement *element, GmxUtil &gmxUtil) const;
};

#endif // MAKE_MATERIAL_H
