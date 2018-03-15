/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/MakeScaling.h"
#include <xercesc/dom/DOM.hpp>
#include <CLHEP/Geometry/Transform3D.h>
#include <CLHEP/Geometry/Vector3D.h>
#include "GeoModelXml/translate.h"
#include "GeoModelXml/GmxUtil.h"



MakeScaling::MakeScaling() {}

using namespace xercesc;
using namespace HepGeom;

Scale3D MakeScaling::getTransform(const DOMElement *scaling, GmxUtil &gmxUtil) {

const int nParams = 3; 
char const *parName[nParams] = {"x", "y", "z"};
double p[nParams];
char *toRelease;

    for (int i = 0; i < nParams; ++i) {
        toRelease = translate(scaling->getAttribute(translate(parName[i])));
        p[i] = gmxUtil.evaluate(toRelease);
        XMLString::release(&toRelease);
    }

    return Scale3D(p[0], p[1], p[2]);
}
