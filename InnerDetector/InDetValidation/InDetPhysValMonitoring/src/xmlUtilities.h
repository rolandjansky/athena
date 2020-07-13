/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//
//  xmlUtilities.h
//  HDef
//
//  Created by sroe on 15/03/2016.
//

#ifndef xmlUtilities_h
#define xmlUtilities_h
#include <memory>
#include <string>
#include <xercesc/util/PlatformUtils.hpp>       // Initialize, Terminate
#include <xercesc/util/XMLString.hpp>           // transcode
#include <xercesc/dom/DOM.hpp>                  // DOMxxx

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY; // Not sure if usage of Xerces-C++ here is thread safe.
// The following warning message is given if checked:
// warning: Use of static expression 'xercesc_3_1::XMLPlatformUtils::fgMemoryManager'
// of type 'xercesc_3_1::MemoryManager*' within function
// 'toNative(const XMLCh*)::<lambda(char*)>' may not be thread-safe.
//
// https://xerces.apache.org/xerces-c/faq-parse-3.html#faq-6
// Is Xerces-C++ thread-safe?
// The answer is yes if you observe the following rules for using Xerces-C++
// in a multi-threaded environment:
// ... ...

typedef std::basic_string<XMLCh>XercesString;
inline XercesString
fromNative(const char* str) {
  auto xDeleter = [&](XMLCh buf[]) {
                    xercesc::XMLString::release(&buf);
                  };

  std::unique_ptr<XMLCh[], decltype(xDeleter)> ptr(xercesc::XMLString::transcode(str), xDeleter);

  return XercesString(ptr.get());
}

inline XercesString
fromNative(const std::string& str) {
  return fromNative(str.c_str());
}

inline std::string
toNative(const XMLCh* str) {
  auto cDeleter = [&](char buf[]) {
                    xercesc::XMLString::release(&buf);
                  };

  std::unique_ptr<char[], decltype(cDeleter)> ptr(xercesc::XMLString::transcode(str), cDeleter);

  return std::string(ptr.get());
}

inline std::string
toNative(const XercesString& str) {
  return toNative(str.c_str());
}

namespace myXerces {
  struct Lib {
    Lib() {
      xercesc::XMLPlatformUtils::Initialize();
    }

    ~Lib() {
      xercesc::XMLPlatformUtils::Terminate();
    }
  };
}   // namespace myXerces



#endif /* xmlUtilities_h */
