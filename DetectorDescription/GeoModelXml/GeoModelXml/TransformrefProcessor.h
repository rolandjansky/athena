/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO_MODEL_XML_TRANSFORMREF_PROCESSOR_H
#define GEO_MODEL_XML_TRANSFORMREF_PROCESSOR_H

//
//   Processor for logvolref tags.
//

#include "GeoModelXml/ElementProcessor.h"
class GmxUtil;

class TransformrefProcessor: public ElementProcessor {
public:
    void process(const xercesc::DOMElement *element, GmxUtil &gmxUtil, GeoNodeList &toAdd);
};

#endif // TRANSFORMREF_PROCESSOR_H
