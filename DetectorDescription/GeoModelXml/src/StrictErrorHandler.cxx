/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/StrictErrorHandler.h"
#include "GeoModelXml/translate.h"
#include <xercesc/dom/DOMError.hpp>
#include <xercesc/dom/DOMLocator.hpp>
#include <iostream>



StrictErrorHandler::StrictErrorHandler(): m_SawErrors(false) {}
StrictErrorHandler::~StrictErrorHandler() {}

bool StrictErrorHandler::handleError(const xercesc::DOMError &domError) {

std::cout << "Handle an error\n";
std::cerr << "Handle an error\n";


    m_SawErrors = true;

    switch (domError.getSeverity()) {
    case (xercesc::DOMError::DOM_SEVERITY_WARNING):
        std::cerr << "\nWarning at file \n";
        break;
    case (xercesc::DOMError::DOM_SEVERITY_ERROR):
        std::cerr << "\nError at file \n";
        break;
    default:
        std::cerr << "\nFatal Error at file \n";
    }
    std::cerr << translate(domError.getLocation()->getURI())
         << ", line " << domError.getLocation()->getLineNumber()
         << ", char " << domError.getLocation()->getColumnNumber()
         << "\n  Message: " << translate(domError.getMessage()) << std::endl;

    return true;
}

