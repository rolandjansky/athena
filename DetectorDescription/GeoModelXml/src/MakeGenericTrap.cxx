/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *NOT*!!! Automatically generated code from /home/hessey/prog/gmx2geo/makeshape
// GenericTraps would have 17 params, so Vakho built up a vector instead.
// So skeleton was automatically generated, then edited by hand.
//
#include "GeoModelXml/shape/MakeGenericTrap.h"
#include <xercesc/dom/DOM.hpp>
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/GeoGenericTrap.h"
#include "GeoModelXml/translate.h"
#include "GeoModelXml/GmxUtil.h"


using namespace xercesc;

MakeGenericTrap::MakeGenericTrap() {}

const RCBase * MakeGenericTrap::make(const xercesc::DOMElement *element, GmxUtil &gmxUtil) const {
const int nParams = 17; 
char const *parName[nParams] = {"x0", "y0", "x1", "y1", "x2", "y2", "x3", "y3", 
                                "x4", "y4", "x5", "y5", "x6", "y6", "x7", "y7", "zhalflength"};
double p;
char *toRelease;
std::vector<GeoTwoVector> vertices;
double x;
double y;

    for (int i = 0; i < (nParams - 1) / 2; ++i) {
        toRelease = Translate(element->getAttribute(Translate(parName[2 * i])));
        x = gmxUtil.evaluate(toRelease);
        XMLString::release(&toRelease);
        toRelease = Translate(element->getAttribute(Translate(parName[2 * i + 1])));
        y = gmxUtil.evaluate(toRelease);
        XMLString::release(&toRelease);
        vertices.push_back(GeoTwoVector(x, y));
    }
//
//    z-half-length
//
    toRelease = Translate(element->getAttribute(Translate(parName[16])));
    p = gmxUtil.evaluate(toRelease);
    XMLString::release(&toRelease);

    return new GeoGenericTrap(p, vertices);
}
