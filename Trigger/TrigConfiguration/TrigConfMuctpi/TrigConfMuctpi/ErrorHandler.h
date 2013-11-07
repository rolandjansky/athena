// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ErrorHandler.h 360889 2011-04-21 15:06:59Z krasznaa $
#ifndef TRIGCONFMUCTPI_ERRORHANDLER_H
#define TRIGCONFMUCTPI_ERRORHANDLER_H

// Xerces-C include(s):
#include <xercesc/sax/ErrorHandler.hpp>

namespace Muctpi {

   /**
    *  @short Class capable of printing parsing error messages
    *
    *         This class should be used when parsing MuCTPI LUT XML
    *         files to print nice messages about parsing errors.
    *
    *         When using the files of the package there should really be no
    *         parsing errors present, but it's better to be safe than
    *         sorry.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 360889 $
    * $Date: 2011-04-21 17:06:59 +0200 (Thu, 21 Apr 2011) $
    */
   class ErrorHandler : public xercesc::ErrorHandler {

   public:
      /// Function handling "warning" level problems
      virtual void warning   ( const xercesc::SAXParseException& ex );
      /// Function handling "error" level problems
      virtual void error     ( const xercesc::SAXParseException& ex );
      /// Function handling "fatalError" level problems
      virtual void fatalError( const xercesc::SAXParseException& ex );

      /// Function that should reset the internal state of the object
      virtual void resetErrors();

   }; // class ErrorHandler

} // namespace Muctpi

#endif // TRIGCONFMUCTPI_ERRORHANDLER_H
