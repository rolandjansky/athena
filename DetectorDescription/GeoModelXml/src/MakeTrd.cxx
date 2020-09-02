/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Automatically generated code from /home/hessey/prog/gmx2geo/makeshape
#include "GeoModelXml/shape/MakeTrd.h"
#include <xercesc/dom/DOM.hpp>
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelXml/translate.h"
#include "GeoModelXml/GmxUtil.h"

using namespace xercesc;

MakeTrd::MakeTrd() {}

const RCBase * MakeTrd::make(const xercesc::DOMElement *element, GmxUtil &gmxUtil) const {
const int nParams = 5; 
char const *parName[nParams] = {"xhalflength1", "xhalflength2", "yhalflength1", "yhalflength2", "zhalflength"};
double p[nParams];
char *toRelease;

    for (int i = 0; i < nParams; ++i) {
        toRelease = Translate(element->getAttribute(Translate(parName[i])));
        p[i] = gmxUtil.evaluate(toRelease);
        XMLString::release(&toRelease);
    }

    return new GeoTrd(p[0], p[1], p[2], p[3], p[4]);
}
