/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/shape/AddPlane.h"
#include <string>
#include <sstream>

#include <xercesc/dom/DOM.hpp>
#include "xercesc/util/XMLString.hpp"

using namespace xercesc;

void AddPlane::process(const xercesc::DOMElement *element, double &zPlane, double &rMinPlane, double &rMaxPlane) {

    std::istringstream(XMLString::transcode(element->getAttribute(XMLString::transcode("zplane")))) >> zPlane;
    std::istringstream(XMLString::transcode(element->getAttribute(XMLString::transcode("rminplane")))) >> rMinPlane;
    std::istringstream(XMLString::transcode(element->getAttribute(XMLString::transcode("rmaxplane")))) >> rMaxPlane;

    return;
}
