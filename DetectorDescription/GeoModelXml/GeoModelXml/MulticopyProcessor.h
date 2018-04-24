/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO_MODEL_XML_MULTICOPY_PROCESSOR_H
#define GEO_MODEL_XML_MULTICOPY_PROCESSOR_H
//
//   Processor for multicopy tags.
//
#include <string>
#include <map>

#include "GeoModelXml/ElementProcessor.h"
class GmxUtil;

class MulticopyProcessor: public ElementProcessor {
public:
    void process(const xercesc_3_1::DOMElement *element, GmxUtil &gmxUtil, GeoNodeList &toAdd);
private:
    std::map <std::string, GeoNodeList> m_map; 
};

#endif // MULTICOPY_PROCESSOR_H
