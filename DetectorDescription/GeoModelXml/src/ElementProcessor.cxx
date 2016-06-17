/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//   Example element processor. This is useful for debugging etc: it helps 
//   get all elements processed, just printing out their name. Including 
//   children of referenced tags. Can be used for example as the default element
//   processor in the processor registry. 
//
//   Parameters: 
//       The element to be processed;
//       The GmxUtil for access to the Evaluator etc.
//       A vector of *GeoGraphNode to add things to that need to be added to the GeoModel tree 
//
//
#include "GeoModelXml/ElementProcessor.h"

#include <iostream>
#include <string>
#include <xercesc/dom/DOM.hpp>
#include "GeoModelXml/translate.h"

#include "GeoModelXml/GeoNodeList.h"
#include "GeoModelXml/ProcessorRegistry.h"
#include "GeoModelXml/GmxUtil.h"

using namespace xercesc;

ElementProcessor::ElementProcessor() {}

void ElementProcessor::process(const DOMElement *element, GmxUtil & /*gmxUtil*/, GeoNodeList &toAdd) {

    char *name2release = translate(element->getNodeName());
    std::string name(name2release);
    XMLString::release(&name2release);

    std::cerr << "Error!!! Default element processor called for tag-name " << name << std::endl;
// Just to get rid of annoying g++ warning:
    if (element == 0) std::cerr << "DOMelement was uninitialised\n";
    if (toAdd.size() == 999999) std::cerr << "GeoNodeList was 999999 long\n";
}
