/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/StrictErrorHandler.h"
#include "xercesc/util/XMLString.hpp"
#include <xercesc/dom/DOMError.hpp>
#include <xercesc/dom/DOMLocator.hpp>
#include "GeoModelXml/OutputDirector.h"

StrictErrorHandler::StrictErrorHandler(): m_SawErrors(false) {}
StrictErrorHandler::~StrictErrorHandler() {}

using namespace xercesc;

bool StrictErrorHandler::handleError(const xercesc::DOMError &domError) {

    OUTPUT_STREAM;
    msglog << MSG::ERROR << "StrictErrorHandler: Handle an error\n";

    m_SawErrors = true;

    switch (domError.getSeverity()) {
    case (xercesc::DOMError::DOM_SEVERITY_WARNING):
        msglog << "\nWarning at file \n";
        break;
    case (xercesc::DOMError::DOM_SEVERITY_ERROR):
        msglog << "\nError at file \n";
        break;
    default:
        msglog << "\nFatal Error at file \n";
    }
    msglog << XMLString::transcode(domError.getLocation()->getURI())
         << ", line " << domError.getLocation()->getLineNumber()
         << ", char " << domError.getLocation()->getColumnNumber()
         << "\n  Message: " << XMLString::transcode(domError.getMessage()) << endmsg;

    return true;
}
