/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//
//   Processor for index elements
//
#include <map>

#include <xercesc/dom/DOM.hpp>
#include "GeoModelXml/IndexProcessor.h"

#include "xercesc/util/XMLString.hpp"
#include "GeoModelXml/GmxUtil.h"

using namespace std;
using namespace xercesc;

void IndexProcessor::process(const DOMElement *element, GmxUtil &gmxUtil, GeoNodeList &/* toAdd */) {

    XMLCh * ref_tmp = XMLString::transcode("ref");
    XMLCh * value_tmp = XMLString::transcode("value");

    char *name2release;

    name2release = XMLString::transcode(element->getAttribute(ref_tmp));
    string name(name2release);
    XMLString::release(&name2release);

    name2release = XMLString::transcode(element->getAttribute(value_tmp));
    string value(name2release);
    XMLString::release(&name2release);

    gmxUtil.positionIndex.setFormula(name, value);

    XMLString::release(&ref_tmp);
    XMLString::release(&value_tmp);

    return;
}
