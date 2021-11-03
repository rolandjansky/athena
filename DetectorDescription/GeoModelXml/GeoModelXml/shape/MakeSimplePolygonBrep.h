/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include <xercesc/util/XercesDefs.hpp>
//
//    Handle simplePolygonBrep shape elements.
//
#ifndef GEO_MODEL_XML_MAKE_SIMPLEPOLYGONBREP_H
#define GEO_MODEL_XML_MAKE_SIMPLEPOLYGONBREP_H
#include <xercesc/util/XercesDefs.hpp>

#include "GeoModelXml/Element2GeoItem.h"

class MakeSimplePolygonBrep: public Element2GeoItem {
public:
    MakeSimplePolygonBrep();
    virtual RCBase * make(const xercesc::DOMElement *element, GmxUtil &gmxUtil) const override;
};

#endif // GEO_MODEL_XML_MAKE_SIMPLEPOLYGONBREP_H
