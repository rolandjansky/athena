/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/MakeRotation.h"
#include <xercesc/dom/DOM.hpp>
#include <CLHEP/Geometry/Transform3D.h>
#include <CLHEP/Geometry/Vector3D.h>
#include "GeoModelXml/translate.h"
#include "GeoModelXml/GmxUtil.h"


MakeRotation::MakeRotation() {}

using namespace xercesc;
using namespace HepGeom;

Rotate3D MakeRotation::getTransform(const DOMElement *rotation, GmxUtil &gmxUtil) {

const int nParams = 4; 
char const *parName[nParams] = {"angle", "xcos", "ycos", "zcos"};
double p[nParams];
char *toRelease;

    for (int i = 0; i < nParams; ++i) {
        toRelease = translate(rotation->getAttribute(translate(parName[i])));
        p[i] = gmxUtil.evaluate(toRelease);
        XMLString::release(&toRelease);
    }

    return Rotate3D(p[0], Vector3D<double>(p[1], p[2], p[3]));
}
