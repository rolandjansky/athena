/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/ProcessorRegistry.h"
#include "GeoModelXml/ElementProcessor.h"

ProcessorRegistry::ProcessorRegistry() {
    m_defaultProcessor = new ElementProcessor(); 
}

void ProcessorRegistry::enregister(const std::string tagName, ElementProcessor *processor) {
    m_processor[tagName] = processor;
}

ElementProcessor * ProcessorRegistry::find(const std::string name) {
    std::map<std::string, ElementProcessor *>::iterator it = m_processor.find(name);

    return it == m_processor.end()? m_defaultProcessor: it->second;
}
