/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/MakeElement.h"
#include <xercesc/dom/DOM.hpp>
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/Units.h"
#include "GeoModelKernel/Units.h"
#include "GeoModelKernel/GeoElement.h"
#include "GeoModelXml/translate.h"
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
    z = Translate(element->getAttribute(Translate("Z")));
    zVal = gmxUtil.evaluate(z);
    XMLString::release(&z);
//
//   Get my A
//
    a = Translate(element->getAttribute(Translate("A")));
    aVal = gmxUtil.evaluate(a);
	
    XMLString::release(&a);
//
//    Get my names
//
    name = Translate(element->getAttribute(Translate("name")));
    shortname = Translate(element->getAttribute(Translate("shortname")));
//
//    Create it
//
    aVal *= gram/mole;
    GeoElement *el = new GeoElement(name, shortname, zVal, aVal);
    XMLString::release(&name);
    XMLString::release(&shortname);

    return (const RCBase *) el;
}
