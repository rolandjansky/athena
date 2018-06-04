/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
