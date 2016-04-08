/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------

#include "xercesc/sax/SAXParseException.hpp"
#include "CoralBase/MessageStream.h"
#include "XMLDOMErrorMessanger.h"
#if XERCES_VERSION_MAJOR < 3
#include "xercesc/dom/deprecated/DOMString.hpp"
#endif

#include <iostream>
#include <string>
#include <stdexcept>

using namespace xercesc;

// Global streaming operator for DOMString is defined in DOMPrint.cpp
//extern ostream& operator<<(ostream& target, const DOMString& s);


#if XERCES_VERSION_MAJOR < 3
namespace pool{  
  inline std::string _toString(const DOMString &toTranscode)
  {
    char * buff = toTranscode.transcode();
    std::string tmp(buff);
    XMLString::release(&buff);
    return tmp;
  }
}
#endif

pool::XMLDOMErrorMessanger::XMLDOMErrorMessanger() :
  fSawErrors(false)
{}

void
pool::XMLDOMErrorMessanger::warning(const SAXParseException&)
{
  //
  // Ignore all warnings.
  //
}

void
pool::XMLDOMErrorMessanger::error(const SAXParseException& toCatch)
{
#if XERCES_VERSION_MAJOR < 3
  std::string message = pool::_toString(DOMString(toCatch.getMessage()));
#else
  // See http://xerces.apache.org/xerces-c/program-dom-3.html
  std::string message( XMLString::transcode(toCatch.getMessage()) );
#endif
  if (message.find("The values for attribute 'name' must be names or name tokens")!=std::string::npos ||
      message.find("The values for attribute 'ID' must be names or name tokens")!=std::string::npos   ||
      message.find("for attribute 'name' is invalid Name or NMTOKEN value")!=std::string::npos                 ||
      message.find("for attribute 'ID' is invalid Name or NMTOKEN value")!=std::string::npos ||
      message.find("for attribute 'name' must be Name or Nmtoken")!=std::string::npos                 ||
      message.find("for attribute 'ID' must be Name or Nmtoken")!=std::string::npos
    )
  {
    return;
  }

  fSawErrors = true;
  coral::MessageStream xmllog( "XMLDOMErrorMessanger");
#if XERCES_VERSION_MAJOR < 3
  xmllog << coral::Error << "Error at file \""  
         << pool::_toString(DOMString(toCatch.getSystemId()))
         << "\", line " << toCatch.getLineNumber()
         << ", column " << toCatch.getColumnNumber()
         << "\n   Message: "    << message
         << coral::MessageStream::endmsg;
#else
  // See http://xerces.apache.org/xerces-c/program-dom-3.html
  xmllog << coral::Error << "Error at file \""  
         << std::string(XMLString::transcode(toCatch.getSystemId()))
         << "\", line " << toCatch.getLineNumber()
         << ", column " << toCatch.getColumnNumber()
         << "\n   Message: "    << message
         << coral::MessageStream::endmsg;
#endif
}

void
pool::XMLDOMErrorMessanger::fatalError(const SAXParseException& toCatch)
{
  fSawErrors = true;
  coral::MessageStream xmllog( "XMLDOMErrorMessanger");
#if XERCES_VERSION_MAJOR < 3
  xmllog << coral::Fatal << "Fatal Error at file \"" 
         << pool::_toString(DOMString(toCatch.getSystemId()))
         << "\", line " << toCatch.getLineNumber()
         << ", column " << toCatch.getColumnNumber()
         << "\n   Message: " 
         << pool::_toString(DOMString(toCatch.getMessage())) 
         << coral::MessageStream::endmsg;
  //  coral::MessageStream::Status s(coral::MessageStream::Status::ERROR,0,10);
#else
  // See http://xerces.apache.org/xerces-c/program-dom-3.html
  xmllog << coral::Fatal << "Fatal Error at file \"" 
         << std::string(XMLString::transcode(toCatch.getSystemId()))
         << "\", line " << toCatch.getLineNumber()
         << ", column " << toCatch.getColumnNumber()
         << "\n   Message: " 
         << std::string(XMLString::transcode(toCatch.getMessage()))
         << coral::MessageStream::endmsg;
#endif
  throw std::runtime_error( "Standard pool exception : Fatal Error on the DOM Parser" );
}

void
pool::XMLDOMErrorMessanger::resetErrors()
{
  // No-op in this case
}
