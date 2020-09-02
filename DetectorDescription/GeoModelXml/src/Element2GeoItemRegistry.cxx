/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/Element2GeoItemRegistry.h"
#include "GeoModelXml/Element2GeoItem.h"

Element2GeoItemRegistry::Element2GeoItemRegistry() {
    m_defaultProcessor = new Element2GeoItem; 
}

Element2GeoItemRegistry::~Element2GeoItemRegistry() {
    delete m_defaultProcessor; 
}

void Element2GeoItemRegistry::enregister(const std::string tagName, Element2GeoItem *processor) {
    m_processor[tagName] = processor;
}

Element2GeoItem * Element2GeoItemRegistry::find(const std::string name) {
    std::map<std::string, Element2GeoItem *>::iterator it = m_processor.find(name);

    return it == m_processor.end()? m_defaultProcessor: it->second;
}
