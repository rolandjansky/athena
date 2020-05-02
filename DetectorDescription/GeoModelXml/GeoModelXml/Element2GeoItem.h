/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//    Base class to process one element (not its children).
//    For each element-tag, create a class inheriting from this; and supply the process method.
//    Register your processor with the BranchProcessor.
//
#ifndef GEO_MODEL_XML_ELEMENT2GEO_ITEM_H
#define GEO_MODEL_XML_ELEMENT2GEO_ITEM_H

#include <map>
#include <string>

#include <xercesc/dom/DOM.hpp>

class RCBase;
class GmxUtil;

class ProcessorRegistry;
namespace HepTool {class Evaluator;}

class Element2GeoItem {

public:
    Element2GeoItem();
    virtual ~Element2GeoItem();
    const RCBase * process(const xercesc::DOMElement *element, GmxUtil &gmxUtil);
    virtual const RCBase * make(const xercesc::DOMElement *element, GmxUtil &gmxUtil) const;
protected:
    std::map<std::string, const RCBase *> m_map;  
};
#endif // ELEMENT2GEO_ITEM_H
