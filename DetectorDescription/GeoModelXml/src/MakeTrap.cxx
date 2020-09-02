/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Automatically generated code from /home/hessey/prog/gmx2geo/makeshape
#include "GeoModelXml/shape/MakeTrap.h"
#include <xercesc/dom/DOM.hpp>
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelXml/translate.h"
#include "GeoModelXml/GmxUtil.h"

using namespace xercesc;

MakeTrap::MakeTrap() {}

const RCBase * MakeTrap::make(const xercesc::DOMElement *element, GmxUtil &gmxUtil) const {
const int nParams = 11; 
char const *parName[nParams] = {"zhalflength", "theta", "phi", "dydzn", "dxdyndzn", "dxdypdzn", "angleydzn", "dydzp", "dxdyndzp", "dxdypdzp", "angleydzp"};
double p[nParams];
char *toRelease;

    for (int i = 0; i < nParams; ++i) {
        toRelease = Translate(element->getAttribute(Translate(parName[i])));
        p[i] = gmxUtil.evaluate(toRelease);
        XMLString::release(&toRelease);
    }

    return new GeoTrap(p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10]);
}
