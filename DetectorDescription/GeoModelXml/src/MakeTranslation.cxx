/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/MakeTranslation.h"
#include <xercesc/dom/DOM.hpp>
//#include <CLHEP/Geometry/Transform3D.h>
//#include <CLHEP/Geometry/Vector3D.h>
#include "GeoModelXml/translate.h"
#include "GeoModelXml/GmxUtil.h"

#include "GeoModelKernel/GeoDefinitions.h"

MakeTranslation::MakeTranslation() {}

using namespace xercesc;
//using namespace HepGeom;

GeoTrf::Translate3D MakeTranslation::getTransform(const DOMElement *translation, GmxUtil &gmxUtil) {

const int nParams = 3; 
char const *parName[nParams] = {"x", "y", "z"};
double p[nParams];
char *toRelease;

    for (int i = 0; i < nParams; ++i) {
        toRelease = Translate(translation->getAttribute(Translate(parName[i])));
        p[i] = gmxUtil.evaluate(toRelease);
        XMLString::release(&toRelease);
    }
    GeoTrf::Translate3D temp(p[0], p[1], p[2]);
    return temp;
}
