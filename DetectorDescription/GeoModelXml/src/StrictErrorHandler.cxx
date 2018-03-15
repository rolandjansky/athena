/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/StrictErrorHandler.h"
#include "GeoModelXml/translate.h"
#include <xercesc/dom/DOMError.hpp>
#include <xercesc/dom/DOMLocator.hpp>
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

StrictErrorHandler::StrictErrorHandler(): m_SawErrors(false) {}
StrictErrorHandler::~StrictErrorHandler() {}

bool StrictErrorHandler::handleError(const xercesc::DOMError &domError) {

    ServiceHandle<IMessageSvc> msgh("MessageSvc", "GeoModelXml");
    MsgStream log(&(*msgh), "GeoModelXml");
    log << MSG::ERROR << "StrictErrorHandler: Handle an error\n";

    m_SawErrors = true;

    switch (domError.getSeverity()) {
    case (xercesc::DOMError::DOM_SEVERITY_WARNING):
        log << "\nWarning at file \n";
        break;
    case (xercesc::DOMError::DOM_SEVERITY_ERROR):
        log << "\nError at file \n";
        break;
    default:
        log << "\nFatal Error at file \n";
    }
    log << translate(domError.getLocation()->getURI())
         << ", line " << domError.getLocation()->getLineNumber()
         << ", char " << domError.getLocation()->getColumnNumber()
         << "\n  Message: " << translate(domError.getMessage()) << endmsg;

    return true;
}
