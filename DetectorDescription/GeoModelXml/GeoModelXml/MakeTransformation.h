/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//    Handle material elements.
//
#ifndef GEO_MODEL_XML_MAKE_TRANSFORMATION_H
#define GEO_MODEL_XML_MAKE_TRANSFORMATION_H

#include "GeoModelXml/Element2GeoItem.h"

class MakeTransformation: public Element2GeoItem {
public:
    MakeTransformation();
    const RCBase * make(const xercesc::DOMElement *element, GmxUtil &gmxUtil) const;
};

#endif // MAKE_TRANSFORMATION_H
