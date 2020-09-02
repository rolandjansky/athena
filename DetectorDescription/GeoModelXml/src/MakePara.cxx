/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Automatically generated code from /home/hessey/prog/gmx2geo/makeshape
#include "GeoModelXml/shape/MakePara.h"
#include <xercesc/dom/DOM.hpp>
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelXml/translate.h"
#include "GeoModelXml/GmxUtil.h"

using namespace xercesc;

MakePara::MakePara() {}

const RCBase * MakePara::make(const xercesc::DOMElement *element, GmxUtil &gmxUtil) const {
const int nParams = 6; 
char const *parName[nParams] = {"xhalflength", "yhalflength", "zhalflength", "alpha", "theta", "phi"};
double p[nParams];
char *toRelease;

    for (int i = 0; i < nParams; ++i) {
        toRelease = Translate(element->getAttribute(Translate(parName[i])));
        p[i] = gmxUtil.evaluate(toRelease);
        XMLString::release(&toRelease);
    }

    return new GeoPara(p[0], p[1], p[2], p[3], p[4], p[5]);
}
