/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ErrorHandler.cxx 360889 2011-04-21 15:06:59Z krasznaa $

// System include(s):
#include <cstdlib>

// Xerces-C include(s):
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/util/XMLString.hpp>

// Local include(s):
#include "TrigConfMuctpi/ErrorHandler.h"
#include "Logging.h"

namespace Muctpi {

   void ErrorHandler::warning( const xercesc::SAXParseException& ex ) {

      char* systemId = xercesc::XMLString::transcode( ex.getSystemId() );
      char* message  = xercesc::XMLString::transcode( ex.getMessage() );

      REPORT_WARNING( "Muctpi::ErrorHandler",
                      "Parsing error occured with parameters:" );
      REPORT_WARNING( "Muctpi::ErrorHandler",
                      "  SystemID: " << systemId );
      REPORT_WARNING( "Muctpi::ErrorHandler",
                      "  Line    : " << ex.getLineNumber() );
      REPORT_WARNING( "Muctpi::ErrorHandler",
                      "  Column  : " << ex.getColumnNumber() );
      REPORT_WARNING( "Muctpi::ErrorHandler",
                      "  Message : " << message );

      xercesc::XMLString::release( &systemId );
      xercesc::XMLString::release( &message );

      return;
   }

   void ErrorHandler::error( const xercesc::SAXParseException& ex ) {

      char* systemId = xercesc::XMLString::transcode( ex.getSystemId() );
      char* message  = xercesc::XMLString::transcode( ex.getMessage() );

      REPORT_ERROR( "Muctpi::ErrorHandler",
                    "Parsing error occured with parameters:" );
      REPORT_ERROR( "Muctpi::ErrorHandler",
                    "  SystemID: " << systemId );
      REPORT_ERROR( "Muctpi::ErrorHandler",
                    "  Line    : " << ex.getLineNumber() );
      REPORT_ERROR( "Muctpi::ErrorHandler",
                    "  Column  : " << ex.getColumnNumber() );
      REPORT_ERROR( "Muctpi::ErrorHandler",
                    "  Message : " << message );

      xercesc::XMLString::release( &systemId );
      xercesc::XMLString::release( &message );

      return;
   }

   void ErrorHandler::fatalError( const xercesc::SAXParseException& ex ) {

      char* systemId = xercesc::XMLString::transcode( ex.getSystemId() );
      char* message  = xercesc::XMLString::transcode( ex.getMessage() );

      REPORT_FATAL( "Muctpi::ErrorHandler",
                    "Parsing error occured with parameters:" );
      REPORT_FATAL( "Muctpi::ErrorHandler",
                    "  SystemID: " << systemId );
      REPORT_FATAL( "Muctpi::ErrorHandler",
                    "  Line    : " << ex.getLineNumber() );
      REPORT_FATAL( "Muctpi::ErrorHandler",
                    "  Column  : " << ex.getColumnNumber() );
      REPORT_FATAL( "Muctpi::ErrorHandler",
                    "  Message : " << message );

      xercesc::XMLString::release( &systemId );
      xercesc::XMLString::release( &message );

      ::exit( 1 );

      return;
   }

   void ErrorHandler::resetErrors() {

      return;
   }

} // namespace Muctpi
