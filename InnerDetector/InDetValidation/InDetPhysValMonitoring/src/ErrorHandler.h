//
//  ErrorHandler.h
//  HDef
//
//  Created by sroe on 18/03/2016.
//  Copyright © 2016 sroe. All rights reserved.
//

#ifndef ErrorHandler_h
#define ErrorHandler_h
#include <stdexcept>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <iostream>
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
