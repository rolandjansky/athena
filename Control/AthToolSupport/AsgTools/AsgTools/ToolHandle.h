// Dear emacs, this is -*- c++ -*-
// $Id: ToolHandle.h 804869 2017-05-15 20:14:34Z krumnack $
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

class INamedInterface;

/// Non-templated base class for the ToolHandle types
///
/// This base class takes care of implementing all the functionality of
/// ToolHandle that doesn't depend on the template parameter of the class.
/// Making the built binaries a bit smaller, and faster.
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// $Revision: 804869 $
/// $Date: 2017-05-15 22:14:34 +0200 (Mon, 15 May 2017) $
///
class ToolHandleBase {

public:
   /// Constructor with a type/name string
   ToolHandleBase( const std::string& typeAndName = "", INamedInterface* parent = 0 );

   /// Return the type/name string specified by the user
   const std::string& typeAndName() const;
   /// Return the tool's type name
   const std::string& type() const;
   /// Return the tool's instance name
   const std::string& name() const;
   /// Return the tool's parent tool name
   const std::string& parentName() const;

   /// Change the handle's type and name
   void setTypeAndName(const std::string& typeAndName);
   /// Change the handle's name
   void setName(const std::string& name);

protected:
   /// The type/name string specified in the constructor
   std::string m_typeAndName;
   /// The type name of the target tool
   std::string m_type;
   /// The instance name of the target tool
   std::string m_name;
   /// The parent tool
   INamedInterface *m_parent;

}; // class ToolHandleBase

/// Partial re-implementation of Gaudi's ToolHandle class
///
/// This class can be used in a dual-use tool to find other dual-use tools
/// that have been instantiated by the user.
///
/// @author David Adams <dladams@bnl.gov>
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// $Revision: 804869 $
/// $Date: 2017-05-15 22:14:34 +0200 (Mon, 15 May 2017) $
///
template< class T >
class ToolHandle : public ToolHandleBase {

public:
   /// A convenience type declaration
   typedef T value_type;

   /// Constructor from a tool pointer
   ToolHandle( T* ptool = 0 );

   /// Constructor from a tool name.
   ToolHandle( const std::string& toolname, INamedInterface* parent = 0 );

   /// Dereferencing operator
   T& operator*();
   /// Dereferencing operator
   const T& operator*() const;
   /// Dereferencing operator
   T* operator->();
   /// Dereferencing operator
   const T* operator->() const;

   /// Retrieve tool.
   /// For compatibility with Gaudi.
   /// Returns success if pointer is non-null and of the correct type.
   StatusCode retrieve() const;

   /// Return true if tool has no pointer or name
   bool empty() const;

   /// Return if the pointer is set
   bool isSet() const;

private:
   /// Pointer to the tool
   mutable T* m_ptool;

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
