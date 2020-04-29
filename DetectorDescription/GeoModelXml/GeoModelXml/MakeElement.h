/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//    Handle material elements.
//
#ifndef GEO_MODEL_XML_MAKE_ELEMENT_H
#define GEO_MODEL_XML_MAKE_ELEMENT_H

#include "GeoModelXml/Element2GeoItem.h"

class MakeElement: public Element2GeoItem {
public:
    MakeElement();
    const RCBase * make(const xercesc::DOMElement *element, GmxUtil &gmxUtil) const;
};

#endif // MAKE_ELEMENT_H
