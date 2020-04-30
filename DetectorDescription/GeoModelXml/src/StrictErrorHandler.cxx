/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/StrictErrorHandler.h"
#include "GeoModelXml/translate.h"
#include <xercesc/dom/DOMError.hpp>
#include <xercesc/dom/DOMLocator.hpp>

#ifndef STANDALONE_GMX
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#else
#include <iostream>
#endif

StrictErrorHandler::StrictErrorHandler(): m_SawErrors(false) {}
StrictErrorHandler::~StrictErrorHandler() {}

bool StrictErrorHandler::handleError(const xercesc::DOMError &domError) {

#ifndef STANDALONE_GMX
    ServiceHandle<IMessageSvc> msgh("MessageSvc", "GeoModelXml");
    MsgStream log(&(*msgh), "GeoModelXml");
    log << MSG::ERROR << "StrictErrorHandler: Handle an error\n";
#else
    std::cout << "StrictErrorHandler: Handle an error\n";
#endif

    m_SawErrors = true;

    switch (domError.getSeverity()) {
    case (xercesc::DOMError::DOM_SEVERITY_WARNING):
#ifndef STANDALONE_GMX
        log << "\nWarning at file \n";
#else
	std::cout<< "\nWarning at file \n";
#endif
        break;
    case (xercesc::DOMError::DOM_SEVERITY_ERROR):
#ifndef STANDALONE_GMX
        log << "\nError at file \n";
#else
	std::cout<< "\nError at file \n";
#endif
        break;
    default:
#ifndef STANDALONE_GMX
        log << "\nFatal Error at file \n";
    }
    log << Translate(domError.getLocation()->getURI())
         << ", line " << domError.getLocation()->getLineNumber()
         << ", char " << domError.getLocation()->getColumnNumber()
         << "\n  Message: " << Translate(domError.getMessage()) << endmsg;
#else
    std::cout << "\nFatal Error at file \n";
    }
    std::cout << Translate(domError.getLocation()->getURI())
         << ", line " << domError.getLocation()->getLineNumber()
         << ", char " << domError.getLocation()->getColumnNumber()
         << "\n  Message: " << Translate(domError.getMessage()) << std::endl;
#endif

    return true;
}
