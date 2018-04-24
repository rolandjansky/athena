/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/MakeElement.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include <xercesc/dom/DOM.hpp>
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/GeoElement.h"
#include "GeoModelXml/translate.h"
#include "GeoModelXml/GmxUtil.h"

using namespace xercesc;
using namespace CLHEP;

MakeElement::MakeElement() {}

const RCBase * MakeElement::make(const xercesc_3_1::DOMElement *element, GmxUtil &gmxUtil) const {
char *name, *shortname;
char *z;
char *a;
double zVal, aVal;
//
//   Get my Z
//
    z = translate(element->getAttribute(translate("Z")));
    zVal = gmxUtil.evaluate(z);
    XMLString::release(&z);
//
//   Get my A
//
    a = translate(element->getAttribute(translate("A")));
    aVal = gmxUtil.evaluate(a);
	
    XMLString::release(&a);
//
//    Get my names
//
    name = translate(element->getAttribute(translate("name")));
    shortname = translate(element->getAttribute(translate("shortname")));
//
//    Create it
//
    aVal *= gram/mole;
    GeoElement *el = new GeoElement(name, shortname, zVal, aVal);
    XMLString::release(&name);
    XMLString::release(&shortname);

    return (const RCBase *) el;
}
