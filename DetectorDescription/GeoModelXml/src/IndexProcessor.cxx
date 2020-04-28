/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//   Processor for index elements
//
#include <map>

#include <xercesc/dom/DOM.hpp>
#include "GeoModelXml/IndexProcessor.h"

#include "GeoModelXml/translate.h"
#include "GeoModelXml/GmxUtil.h"

using namespace std;
using namespace xercesc;

void IndexProcessor::process(const DOMElement *element, GmxUtil &gmxUtil, GeoNodeList &/* toAdd */) {

    char *name2release;

    name2release = Translate(element->getAttribute(Translate("ref")));
    string name(name2release);
    XMLString::release(&name2release);

    name2release = Translate(element->getAttribute(Translate("value")));
    string value(name2release);
    XMLString::release(&name2release);

    gmxUtil.positionIndex.setFormula(name, value);

    return;
}
