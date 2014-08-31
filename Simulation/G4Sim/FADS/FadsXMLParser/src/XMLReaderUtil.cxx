/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsXMLParser/XMLReaderUtil.h"
#include <xercesc/util/PlatformUtils.hpp>
#include <iostream>
XERCES_CPP_NAMESPACE_USE

namespace FADS {

  std::string& XMLReaderUtil::Transcode(const XMLCh* t)
  {
    static std::string s="";
    char *p = XMLString::transcode(t);
    s=p;
    XMLString::release(&p,XMLPlatformUtils::fgMemoryManager);
    //delete [] p;
    return s;
  }


  std::string XMLReaderUtil::trim (const std::string s)
  //-------------------------------------------------------------
  {
    static const std::string null_string;
    if (s.size () == 0) return (s);
    std::string temp = s;
    std::string::size_type i;
    i = temp.find_first_not_of (' ');
    if (i == std::string::npos) return (null_string);
    // There is at least 1 non blank character in s.
    if (i > 0)
      {
        temp = temp.substr (i);
      }
    i = temp.find_last_not_of (' ');
    if (i < temp.size ())
      {
        temp = temp.substr (0, i + 1);
      }
    return (temp);
  }

} // end namespace
