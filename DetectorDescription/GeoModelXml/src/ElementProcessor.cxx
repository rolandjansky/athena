/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//   Example element processor. This is useful for debugging etc: it helps 
//   get all elements processed, just printing out their name. Including 
//   children of referenced tags. Can be used for example as the default element
//   processor in the processor registry. 
//
//   Parameters: 
//       The element to be processed;
//       The GmxUtil for access to the Evaluator etc.
//       A vector of *GeoGraphNode to add things to that need to be added to the GeoModel tree 
//
//
#include "GeoModelXml/ElementProcessor.h"

#ifndef STANDALONE_GMX
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#else
#include <iostream>
#endif


#include <string>
#include <xercesc/dom/DOM.hpp>
#include "GeoModelXml/translate.h"

#include "GeoModelXml/GeoNodeList.h"
#include "GeoModelXml/ProcessorRegistry.h"
#include "GeoModelXml/GmxUtil.h"

using namespace xercesc;

ElementProcessor::ElementProcessor() {}

void ElementProcessor::process(const DOMElement *element, GmxUtil & /* gmxUtil*/, GeoNodeList & /* toAdd */) {

    char *name2release = Translate(element->getNodeName());
    std::string name(name2release);
    XMLString::release(&name2release);

#ifndef STANDALONE_GMX
    ServiceHandle<IMessageSvc> msgh("MessageSvc", "GeoModelXml");
    MsgStream log(&(*msgh), "GeoModelXml");
    log << MSG::FATAL << "Error!!! Default element processor called for tag-name " << name << endmsg;
#else
    std::cerr << "Error!!! Default element processor called for tag-name " << name << std::endl;
#endif
}
