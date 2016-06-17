/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//    Process an element followed by all its children.
//    Element processors have to be registered with me so I know who to call.
//    Any element tag with no registered processor will be handled by a default handler, which
//    does nothing and stops processing of its children.
//
#ifndef GEO_MODEL_XML_ELEMENT2GEO_ITEM_REGISTRY_H
#define GEO_MODEL_XML_ELEMENT2GEO_ITEM_REGISTRY_H
#include <string>
#include <map>

class Element2GeoItem;

class Element2GeoItemRegistry {
public:
    Element2GeoItemRegistry();
    ~Element2GeoItemRegistry();
    void enregister(const std::string tagName,  Element2GeoItem *processor); // register = keyword
    Element2GeoItem *find(const std::string tagName);

private:
    Element2GeoItem *m_defaultProcessor;
    std::map<std::string,  Element2GeoItem *> m_processor;
};

#endif // ELEMENT2GEO_ITEM_REGISTRY_H
