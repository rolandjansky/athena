/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "XincludeErrHandler.h"
#include "xmlUtilities.h"
#include <iostream>

XIncludeErrHandler::XIncludeErrHandler() : m_errors(false) {
}

XIncludeErrHandler::~XIncludeErrHandler() {
}

bool
XIncludeErrHandler::handleError(const xercesc::DOMError& domError) {
  bool continueParsing = true;

  if (domError.getSeverity() == xercesc::DOMError::DOM_SEVERITY_WARNING) {
    std::cout << "\nWarning at file ";
  } else if (domError.getSeverity() == xercesc::DOMError::DOM_SEVERITY_ERROR) {
    std::cout << "\nError at file ";
    m_errors = true;
  } else {
    std::cout << "\nFatal Error at file ";
    continueParsing = false;
    m_errors = true;
  }

  std::cout << toNative(domError.getLocation()->getURI())
            << ", line " << domError.getLocation()->getLineNumber()
            << ", char " << domError.getLocation()->getColumnNumber()
            << "\n  Message: " << toNative(domError.getMessage()) << std::endl;

  return continueParsing;
}

void
XIncludeErrHandler::resetErrors() {
  m_errors = false;
}
