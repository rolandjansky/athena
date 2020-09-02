/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/shape/AddPlane.h"
#include <string>
#include <sstream>

#include <xercesc/dom/DOM.hpp>
#include "GeoModelXml/translate.h"

using namespace xercesc;

void AddPlane::process(const DOMElement *element, double &zPlane, double &rMinPlane, double &rMaxPlane) {

    std::istringstream(Translate(element->getAttribute(Translate("zplane")))) >> zPlane;
    std::istringstream(Translate(element->getAttribute(Translate("rminplane")))) >> rMinPlane;
    std::istringstream(Translate(element->getAttribute(Translate("rmaxplane")))) >> rMaxPlane;

    return;
}
