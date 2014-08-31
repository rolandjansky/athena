/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XMLReaderUtil_H
#define XMLReaderUtil_H

#include <string>
#include <xercesc/util/XMLString.hpp>

namespace FADS {

  class XMLReaderUtil {
  public:
    static std::string& Transcode(const XMLCh* t);
    static std::string trim (const std::string s);
  };

} // end namespace

#endif
