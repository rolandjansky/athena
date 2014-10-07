// Dear emacs, this is -*- c++ -*-
// $Id: ToolStore.h 620407 2014-10-07 13:40:28Z krasznaa $
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
   /// $Revision: 620407 $
   /// $Date: 2014-10-07 15:40:28 +0200 (Tue, 07 Oct 2014) $
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
      static IAsgTool* get( const std::string& name );

      /// Retrieve a tool by name and interface.
      template< typename T >
      static T* get( const std::string& name ) {

         return dynamic_cast< T* >( get( name ) );
      }

      /// Remove the specified tool from the store
      static StatusCode remove( const IAsgTool* tool );
      /// Remove a tool with a given name from the store
      static StatusCode remove( const std::string& name );

   }; // class ToolStore

} // namespace asg

#endif // ASGTOOLS_TOOLSTORE_H
