/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//    Process an element followed by all its children.
//    Element processors have to be registered with me so I know who to call.
//    Any element tag with no registered processor will be handled by a default handler, which
//    does nothing and stops processing of its children.
//
#ifndef GEO_MODEL_XML_PROCESSOR_REGISTRY_H
#define GEO_MODEL_XML_PROCESSOR_REGISTRY_H

#include <string>
#include <map>

class ElementProcessor;

class ProcessorRegistry {
public:
    ProcessorRegistry();
    void enregister(const std::string tagName, ElementProcessor *processor);
    ElementProcessor *find(const std::string name);

private:
    ElementProcessor *m_defaultProcessor;
    std::map<std::string, ElementProcessor *> m_processor;
};

#endif // PROCESSOR_REGISTRY_H
