/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <xercesc/sax/SAXParseException.hpp>
#include "FadsXMLParser/DOMTreeErrorReporter.h"
#include <iostream>
#include <stdlib.h>
#include <string>

#include "FadsXMLParser/XMLReaderUtil.h"

namespace FADS {

  void DOMTreeErrorReporter::warning(const SAXParseException&)
  {
    //
    // Ignore all warnings.
    //
  }

  void DOMTreeErrorReporter::error(const SAXParseException& toCatch)
  {
    fSawErrors = true;
    std::cerr << "Error at file \"" << XMLReaderUtil::Transcode(toCatch.getSystemId())
              << "\", line " << toCatch.getLineNumber()
              << ", column " << toCatch.getColumnNumber()
              << "\n   Message: " << XMLReaderUtil::Transcode(toCatch.getMessage()) << std::endl;
  }

  void DOMTreeErrorReporter::fatalError(const SAXParseException& toCatch)
  {
    fSawErrors = true;
    std::cerr << "Fatal Error at file \"" << XMLReaderUtil::Transcode(toCatch.getSystemId())
              << "\", line " << toCatch.getLineNumber()
              << ", column " << toCatch.getColumnNumber()
              << "\n   Message: " << XMLReaderUtil::Transcode(toCatch.getMessage()) << std::endl;
  }

  void DOMTreeErrorReporter::resetErrors()
  {
    fSawErrors = false;
  }

} // end namespace
