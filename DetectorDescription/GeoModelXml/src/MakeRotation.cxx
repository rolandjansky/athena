/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/MakeRotation.h"
#include <xercesc/dom/DOM.hpp>
//  #include <CLHEP/Geometry/Transform3D.h>
//  #include <CLHEP/Geometry/Vector3D.h>
#include "GeoModelXml/translate.h"
#include "GeoModelXml/GmxUtil.h"


MakeRotation::MakeRotation() {}

using namespace xercesc;
//  using namespace HepGeom;

GeoTrf::Rotation3D MakeRotation::getTransform(const DOMElement *rotation, GmxUtil &gmxUtil) {

const int nParams = 4; 
char const *parName[nParams] = {"angle", "xcos", "ycos", "zcos"};
double p[nParams];
char *toRelease;

    for (int i = 0; i < nParams; ++i) {
        toRelease = Translate(rotation->getAttribute(Translate(parName[i])));
        p[i] = gmxUtil.evaluate(toRelease);
        XMLString::release(&toRelease);
    }
    GeoTrf::Rotation3D temp;
    temp = GeoTrf::AngleAxis3D(p[0], GeoTrf::Vector3D(p[1], p[2], p[3]));

    return temp;
}
