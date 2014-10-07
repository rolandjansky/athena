// Dear emacs, this is -*- c++ -*-
// $Id: ToolHandle.h 612816 2014-08-21 12:03:06Z krasznaa $
#ifndef ASGTOOLS_TOOLHANDLE_H
#define ASGTOOLS_TOOLHANDLE_H

// Local include(s):
#include "AsgTools/AsgToolsConf.h"

#ifdef ASGTOOL_ATHENA
#   include "GaudiKernel/ToolHandle.h"
#elif defined(ASGTOOL_STANDALONE)

// System include(s):
#include <string>
#include <iosfwd>

// Local include(s):
#include "AsgTools/StatusCode.h"

/// Partial re-implementation of Gaudi's ToolHandle class
///
/// This class can be used in a dual-use tool to find other dual-use tools
/// that have been instantiated by the user.
///
/// @author David Adams <dladams@bnl.gov>
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// $Revision: 612816 $
/// $Date: 2014-08-21 14:03:06 +0200 (Thu, 21 Aug 2014) $
///
template< class T >
class ToolHandle {

public:
   /// A convenience type declaration
   typedef T value_type;

   /// Constructor from a tool pointer
   ToolHandle( T* ptool = 0 );

   /// Constructor from a tool name.
   ToolHandle( const std::string& toolname );

   /// Return the tool's name
   const std::string& name() const;

   /// Dereferencing operator
   T& operator*() const;
   /// Dereferencing operator
   T* operator->() const;

   /// Retrieve tool.
   /// For compatibility with Gaudi.
   /// Returns success if pointer is non-null and of the correct type.
   StatusCode retrieve() const;

   /// Return true if tool has no pointer or name
   bool empty() const;

private:
   /// Pointer to the tool
   mutable T* m_ptool;
   /// The name of the tool
   std::string m_toolname;

}; // class ToolHandle

/// A convenience operator for printing the values of such objects
template< class T >
std::ostream& operator<< ( std::ostream& out,
                           const ToolHandle< T >& handle );

// Include the implementation of the code:
#include "AsgTools/ToolHandle.icc"

#else
#   error "What environment are we in?!?"
#endif // Environment selection
#endif // ASGTOOLS_TOOLHANDLE_H
