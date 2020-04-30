/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Automatically generated from scripts in /home/hessey/prog/gmx2geo/makeshape
//    Handle tube shape elements.
//
#ifndef GEO_MODEL_XML_MAKE_BOX_H
#define GEO_MODEL_XML_MAKE_BOX_H

#include "GeoModelXml/Element2GeoItem.h"

class MakeBox: public Element2GeoItem {
public:
    MakeBox();
    const RCBase * make(const xercesc::DOMElement *element, GmxUtil &gmxUtil) const;
};

#endif // GEO_MODEL_XML_MAKE_BOX_H
