/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//   Processor for addbranch elements
//
//   Process children and get list of things to be added to the tree.
//
#include "GeoModelXml/AddbranchProcessor.h"
#include <map>

#include <xercesc/dom/DOM.hpp>
#include "GeoModelXml/ProcessorRegistry.h"

#include "GeoModelXml/translate.h"
#include "GeoModelXml/GmxUtil.h"

using namespace std;
using namespace xercesc;

void AddbranchProcessor::process(const DOMElement *element, GmxUtil &gmxUtil, GeoNodeList &toAdd) {

    for (DOMNode *child = element->getFirstChild(); child != 0; child = child->getNextSibling()) {
        if (child->getNodeType() == DOMNode::ELEMENT_NODE) {
            char *name2release = Translate(child->getNodeName());
            string name(name2release);
            XMLString::release(&name2release);
            gmxUtil.processorRegistry.find(name)->process(dynamic_cast<const DOMElement *>(child), gmxUtil, toAdd);
        }
    }
}
