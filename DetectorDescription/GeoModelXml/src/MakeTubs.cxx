/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Automatically generated code from /home/hessey/prog/gmx2geo/makeshape
#include "GeoModelXml/shape/MakeTubs.h"
#include <xercesc/dom/DOM.hpp>
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/GeoTubs.h"
#include "xercesc/util/XMLString.hpp"
#include "GeoModelXml/GmxUtil.h"

using namespace xercesc;

MakeTubs::MakeTubs() {}

const RCBase * MakeTubs::make(const xercesc::DOMElement *element, GmxUtil &gmxUtil) const {
const int nParams = 5; 
char const *parName[nParams] = {"rmin", "rmax", "zhalflength", "sphi", "dphi"};
double p[nParams];
char *toRelease;

    for (int i = 0; i < nParams; ++i) {
        toRelease = XMLString::transcode(element->getAttribute(XMLString::transcode(parName[i])));
        p[i] = gmxUtil.evaluate(toRelease);
        XMLString::release(&toRelease);
    }

    return new GeoTubs(p[0], p[1], p[2], p[3], p[4]);
}
