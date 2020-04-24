/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO_MODEL_XML_INDEX_PROCESSOR_H
#define GEO_MODEL_XML_INDEX_PROCESSOR_H
//
//   Processor for index tags which change the formula for generating a position-index.
//
#include "GeoModelXml/ElementProcessor.h"

class IndexProcessor: public ElementProcessor {
public:
    void process(const xercesc::DOMElement *element, GmxUtil &util, GeoNodeList &toAdd);
};

#endif // INDEX_PROCESSOR_H
