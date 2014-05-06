// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CodeGenerator_v2.h 516932 2012-09-10 09:29:08Z krasznaa $
#ifndef D3PDMAKERREADER_CODEGENERATOR_V2_H
#define D3PDMAKERREADER_CODEGENERATOR_V2_H

// STL include(s):
#include <string>
#include <vector>
#include <set>

// Gaudi/Athena include(s):
#include "GaudiKernel/StatusCode.h"

namespace D3PD {

   // Forward declaration(s):
   class ObjectMetadata;

   /**
    *  @short Private namespace for version 2 of the code generator code
    *
    *         The code generator functions have now been separated completely
    *         from the D3PD code. This way it should be possible to generate
    *         the same kind of code from multiple input sources.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 516932 $
    * $Date: 2012-09-10 11:29:08 +0200 (Mon, 10 Sep 2012) $
    */
   namespace Version2 {

      /// Write the "D3PDObjectBase" source files into a given directory
      StatusCode writeD3PDObjectBase( const std::string& dir );
      /// Write the "VarHandle" source files into a given directory
      StatusCode writeVarHandle( const std::string& dir );
      /// Write the "VarProxy" source files into a given direcory
      StatusCode writeVarProxy( const std::string& dir );
      /// Write the UserD3PDObject source files into a given directory
      StatusCode writeUserD3PDObject( const std::string& dir );
      /// Write the "D3PDReadStats" source files into a given directory
      StatusCode writeD3PDReadStats( const std::string& dir );
      /// Write the "D3PDPerfStats" source files into a given directory
      StatusCode writeD3PDPerfStats( const std::string& dir );
      /// Write the "Utils" source files into a given directory
      StatusCode writeUtils( const std::string& dir );

      /// Write the header of the class(es) describing some D3PD variables
      StatusCode writeHeader( const std::string& classname, const std::string& dir,
                              const ObjectMetadata& metadata );
      /// Write the source of the class(es) describing some D3PD variables
      StatusCode writeSource( const std::string& classname, const std::string& dir,
                              const ObjectMetadata& metadata );

      /// Write the header of the main event class describing a D3PD tree
      StatusCode writeEventHeader( const std::string& classname, const std::string& dir,
                                   const std::set< ObjectMetadata >& metadata );
      /// Write the source of the main event class describing a D3PD tree
      StatusCode writeEventSource( const std::string& classname, const std::string& dir,
                                   const std::set< ObjectMetadata >& metadata );

   } // namespace Version2

} // namespace D3PD

#endif // D3PDMAKERREADER_CODEGENERATOR_V2_H
