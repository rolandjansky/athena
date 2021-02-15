/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/MakeElement.h"
#include <xercesc/dom/DOM.hpp>
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/Units.h"
#include "GeoModelKernel/GeoElement.h"
#include "xercesc/util/XMLString.hpp"
#include "GeoModelXml/GmxUtil.h"

using namespace xercesc;
using namespace GeoModelKernelUnits;

MakeElement::MakeElement() {}

const RCBase * MakeElement::make(const xercesc::DOMElement *element, GmxUtil &gmxUtil) const {
char *name, *shortname;
char *z;
char *a;
double zVal, aVal;
//
//   Get my Z
//
    z = XMLString::transcode(element->getAttribute(XMLString::transcode("Z")));
    zVal = gmxUtil.evaluate(z);
    XMLString::release(&z);
//
//   Get my A
//
    a = XMLString::transcode(element->getAttribute(XMLString::transcode("A")));
    aVal = gmxUtil.evaluate(a);
	
    XMLString::release(&a);
//
//    Get my names
//
    name = XMLString::transcode(element->getAttribute(XMLString::transcode("name")));
    shortname = XMLString::transcode(element->getAttribute(XMLString::transcode("shortname")));
//
//    Create it
//
    aVal *= gram/mole;
    GeoElement *el = new GeoElement(name, shortname, zVal, aVal);
    XMLString::release(&name);
    XMLString::release(&shortname);

    return (const RCBase *) el;
}
