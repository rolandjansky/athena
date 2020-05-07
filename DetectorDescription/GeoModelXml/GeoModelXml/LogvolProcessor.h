/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO_MODEL_XML_LOGVOL_PROCESSOR_H
#define GEO_MODEL_XML_LOGVOL_PROCESSOR_H
//
//   Processor for logvol tags.
//
#include <string>
#include <map>

#include "GeoModelXml/ElementProcessor.h"
class GmxUtil;
class GeoNameTag;
class GeoLogVol;

class LogvolProcessor: public ElementProcessor {
public:
    typedef struct {
        GeoNameTag *name;
        int id;
        GeoLogVol *logVol;
        bool alignable;
    } LogVolStore; 
    void process(const xercesc::DOMElement *element, GmxUtil &gmxUtil, GeoNodeList &toAdd);
    void zeroId(const xercesc::DOMElement *element);
private:
    std::map<std::string, LogVolStore> m_map; 
};

#endif // LOGVOL_PROCESSOR_H
