/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//    Base class to process one element (not its children).
//    For each element-tag, create a class inheriting from this; and supply the process method.
//    Register your processor with the BranchProcessor.
//
#ifndef GEO_MODEL_XML_ELEMENT_PROCESSOR_H
#define GEO_MODEL_XML_ELEMENT_PROCESSOR_H
#include <xercesc/util/XercesDefs.hpp>

#include "GeoModelXml/GeoNodeList.h"

XERCES_CPP_NAMESPACE_BEGIN
class DOMElement;
XERCES_CPP_NAMESPACE_END
class GmxUtil;

class ElementProcessor {

public:
    ElementProcessor();
    virtual void process(const xercesc::DOMElement *element, GmxUtil &gmxUtil, 
                         GeoNodeList &toBeAdded);
};
#endif // ELEMENT_PROCESSOR_H
