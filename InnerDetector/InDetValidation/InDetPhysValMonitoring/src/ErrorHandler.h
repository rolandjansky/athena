/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  ErrorHandler.h
//  HDef
//
//  Created by sroe on 18/03/2016.
//

#ifndef ErrorHandler_h
#define ErrorHandler_h
#include <stdexcept>
#include <xercesc/sax2/DefaultHandler.hpp>
class XmlErrorHandler: public xercesc::DefaultHandler {
public:
  void
  warning(const xercesc::SAXParseException& e) {
    std::cout << "warning: " << toNative(e.getMessage()) << std::endl;
  }

  void
  error(const xercesc::SAXParseException& e) {
    throw std::runtime_error(toNative(e.getMessage()));
  }

  void
  fatalError(const xercesc::SAXParseException& e) {
    error(e);
  }
};

#endif /* ErrorHandler_h */
