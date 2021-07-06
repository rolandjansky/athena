/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/shape/AddPlane.h"
#include <string>
#include <sstream>

#include <xercesc/dom/DOM.hpp>
#include "xercesc/util/XMLString.hpp"

using namespace xercesc;

void AddPlane::process(const xercesc::DOMElement *element, double &zPlane, double &rMinPlane, double &rMaxPlane) {

  XMLCh * zplane_tmp = XMLString::transcode("zplane");
  std::istringstream(XMLString::transcode(element->getAttribute(zplane_tmp))) >> zPlane;
  XMLCh * rminplane_tmp = XMLString::transcode("rminplane");
  std::istringstream(XMLString::transcode(element->getAttribute(rminplane_tmp))) >> rMinPlane;
  XMLCh * rmaxplane_tmp = XMLString::transcode("rmaxplane");
  std::istringstream(XMLString::transcode(element->getAttribute(rmaxplane_tmp))) >> rMaxPlane;
  XMLString::release(&zplane_tmp);
  XMLString::release(&rminplane_tmp);
  XMLString::release(&rmaxplane_tmp);
    return;
}
