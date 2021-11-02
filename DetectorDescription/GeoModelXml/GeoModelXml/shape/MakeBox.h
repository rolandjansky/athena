/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//
// Automatically generated from scripts in /home/hessey/prog/gmx2geo/makeshape
//    Handle tube shape elements.
//
#ifndef GEO_MODEL_XML_MAKE_BOX_H
#define GEO_MODEL_XML_MAKE_BOX_H
#include <xercesc/util/XercesDefs.hpp>

#include "GeoModelXml/Element2GeoItem.h"

class MakeBox: public Element2GeoItem {
public:
    MakeBox();
    virtual RCBase * make(const xercesc::DOMElement *element, GmxUtil &gmxUtil) const override;
};

#endif // GEO_MODEL_XML_MAKE_BOX_H
