/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Automatically generated code from /home/hessey/prog/gmx2geo/makeshape
#include "GeoModelXml/shape/MakeCons.h"
#include <xercesc/dom/DOM.hpp>
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelXml/translate.h"
#include "GeoModelXml/GmxUtil.h"

using namespace xercesc;

MakeCons::MakeCons() {}

const RCBase * MakeCons::make(const xercesc::DOMElement *element, GmxUtil &gmxUtil) const {
const int nParams = 7; 
char const *parName[nParams] = {"rmin1", "rmin2", "rmax1", "rmax2", "dz", "sphi", "dphi"};
double p[nParams];
char *toRelease;

    for (int i = 0; i < nParams; ++i) {
        toRelease = Translate(element->getAttribute(Translate(parName[i])));
        p[i] = gmxUtil.evaluate(toRelease);
        XMLString::release(&toRelease);
    }

    return new GeoCons(p[0], p[1], p[2], p[3], p[4], p[5], p[6]);
}
