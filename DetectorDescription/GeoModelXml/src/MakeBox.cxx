/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Automatically generated code from /home/hessey/prog/gmx2geo/makeshape
#include "GeoModelXml/shape/MakeBox.h"
#include <xercesc/dom/DOM.hpp>
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelXml/translate.h"
#include "GeoModelXml/GmxUtil.h"

using namespace xercesc;

MakeBox::MakeBox() {}

const RCBase * MakeBox::make(const xercesc_3_1::DOMElement *element, GmxUtil &gmxUtil) const {
const int nParams = 3; 
char const *parName[nParams] = {"xhalflength", "yhalflength", "zhalflength"};
double p[nParams];
char *toRelease;

    for (int i = 0; i < nParams; ++i) {
        toRelease = translate(element->getAttribute(translate(parName[i])));
        p[i] = gmxUtil.evaluate(toRelease);
        XMLString::release(&toRelease);
    }

    return new GeoBox(p[0], p[1], p[2]);
}
