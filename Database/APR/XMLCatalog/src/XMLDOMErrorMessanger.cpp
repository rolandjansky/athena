/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------

#include "xercesc/sax/SAXParseException.hpp"
#include "XMLDOMErrorMessanger.h"

#include <iostream>
#include <string>
#include <stdexcept>

using namespace xercesc;

// Global streaming operator for DOMString is defined in DOMPrint.cpp
//extern ostream& operator<<(ostream& target, const DOMString& s);


pool::XMLDOMErrorMessanger::XMLDOMErrorMessanger(MsgStream& _msg) :
      fSawErrors(false),
      msg(_msg)
{
}

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
   // See http://xerces.apache.org/xerces-c/program-dom-3.html
   std::string message( XMLString::transcode(toCatch.getMessage()) );
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
   // See http://xerces.apache.org/xerces-c/program-dom-3.html  
   msg << MSG::ERROR << "XML Error in file \""  
       << std::string(XMLString::transcode(toCatch.getSystemId()))
       << "\", line " << toCatch.getLineNumber()
       << ", column " << toCatch.getColumnNumber()
       << "\n   Message: "    << message  << endmsg;
}

void
pool::XMLDOMErrorMessanger::fatalError(const SAXParseException& toCatch)
{
   fSawErrors = true;
   // See http://xerces.apache.org/xerces-c/program-dom-3.html
   msg << MSG::FATAL << "XML Error in file \"" 
       << std::string(XMLString::transcode(toCatch.getSystemId()))
       << "\", line " << toCatch.getLineNumber()
       << ", column " << toCatch.getColumnNumber()
       << "\n   Message: " 
       << std::string(XMLString::transcode(toCatch.getMessage())) << endmsg;
   throw std::runtime_error( "Standard pool exception : Fatal Error on the DOM Parser" );
}

void
pool::XMLDOMErrorMessanger::resetErrors()
{
   // No-op in this case
}
