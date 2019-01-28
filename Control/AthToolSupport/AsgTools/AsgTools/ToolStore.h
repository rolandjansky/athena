// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//
#ifndef ASGTOOLS_TOOLSTORE_H
#define ASGTOOLS_TOOLSTORE_H

// System include(s):
#include <string>

// Local include(s):
#include "AsgTools/IAsgTool.h"
#include "AsgTools/StatusCode.h"

namespace asg {

   /// A light-weight replacement for ToolSvc for ROOT analysis
   ///
   /// In order to allow tools to find each other through ToolHandles,
   /// just like in Athena, we need a global registry of all of the tools
   /// that the user instantiates in his/her job.
   ///
   /// This class does exactly this. All created dual-use tools register
   /// themselves in it, so that other components could later on find them.
   ///
   /// @author David Adams <dladams@bnl.gov>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 802972 $
   /// $Date: 2017-04-15 20:13:17 +0200 (Sat, 15 Apr 2017) $
   ///
   class ToolStore {

   public:
      /// Store a named tool by its name.
      /// The name must be non-blank and not already exist in the store.
      static StatusCode put( IAsgTool* ptool );

      /// Store a tool by an alternate name.
      /// The name must be non-blank and not already exist in the store.
      static StatusCode put( IAsgTool* ptool, const std::string& name );

      /// Retrieve a tool by name.
      static IAsgTool* get( const std::string& name, bool silent = false );

      /// Retrieve a tool by name and interface.
      template< typename T >
      static T* get( const std::string& name ) {

         return dynamic_cast< T* >( get( name ) );
      }

      /// Check whether a tool with a given type and name is known in the store
      template< typename T >
      static bool contains( const std::string& name ) {

         return ( dynamic_cast< T* >( get( name, true ) ) != 0 );
      }

      /// Remove the specified tool from the store
      static StatusCode remove( const IAsgTool* tool );
      /// Remove a tool with a given name from the store
      static StatusCode remove( const std::string& name );

#ifdef XAOD_STANDALONE
     /// dump the tool configuration for all tools to std::cout
     static void dumpToolConfig ();
#endif

   }; // class ToolStore

} // namespace asg

#endif // ASGTOOLS_TOOLSTORE_H
