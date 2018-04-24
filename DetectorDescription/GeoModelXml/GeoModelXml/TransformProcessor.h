/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO_MODEL_XML_TRANSFORM_PROCESSOR_H
#define GEO_MODEL_XML_TRANSFORM_PROCESSOR_H

//
//   Processor for transform tags.
//
#include "GeoModelXml/ElementProcessor.h"
class GmxUtil;

class TransformProcessor: public ElementProcessor {
public:
    void process(const xercesc_3_1::DOMElement *element, GmxUtil &gmxUtil, GeoNodeList &toAdd);
};

#endif // TRANSFORM_PROCESSOR_H