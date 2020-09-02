/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO_MODEL_XML_ASSEMBLY_PROCESSOR_H
#define GEO_MODEL_XML_ASSEMBLY_PROCESSOR_H
//
//   Processor for assembly tags.
//
#include <string>
#include <map>

#include "GeoModelXml/ElementProcessor.h"
class GmxUtil;
class GeoNameTag;

class AssemblyProcessor: public ElementProcessor {
public:
    typedef struct {
        GeoNameTag *name;
        int id;
        bool alignable;
    } AssemblyStore; 
    void process(const xercesc::DOMElement *element, GmxUtil &gmxUtil, GeoNodeList &toAdd);
    void zeroId(const xercesc::DOMElement *element);
private:
    std::map<std::string, AssemblyStore> m_map; 
};

#endif // ASSEMBLY_PROCESSOR_H
