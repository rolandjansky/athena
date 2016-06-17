/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO_MODEL_XML_ADD_BRANCH_PROCESSOR_H
#define GEO_MODEL_XML_ADD_BRANCH_PROCESSOR_H
//
//   Processor for addbranch tags.
//
#include "GeoModelXml/ElementProcessor.h"

class AddbranchProcessor: public ElementProcessor {
public:
    void process(const xercesc_3_1::DOMElement *element, GmxUtil &util, GeoNodeList &toAdd);
};

#endif // ADD_BRANCH_PROCESSOR_H
