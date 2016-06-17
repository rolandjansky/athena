/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/shape/AddPlane.h"
#include <string>
#include <iostream>
#include <sstream>

#include <xercesc/dom/DOM.hpp>
#include "GeoModelXml/translate.h"

void AddPlane::process(const xercesc_3_1::DOMElement *element, double &zPlane, double &rMinPlane, double &rMaxPlane) {

    std::istringstream(translate(element->getAttribute(translate("zplane")))) >> zPlane;
    std::istringstream(translate(element->getAttribute(translate("rminplane")))) >> rMinPlane;
    std::istringstream(translate(element->getAttribute(translate("rmaxplane")))) >> rMaxPlane;

    return;
}