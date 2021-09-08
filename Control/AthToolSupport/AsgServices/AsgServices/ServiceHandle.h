/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack
/// @author David Adams <dladams@bnl.gov> (for original implementation for tools)
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch> (for original implementation for tools)


#ifndef ASGSERVICES_SERVICEHANDLE_H
#define ASGSERVICES_SERVICEHANDLE_H

// Local include(s):
#ifndef XAOD_STANDALONE
#   include "GaudiKernel/ServiceHandle.h"
#else // not XAOD_STANDALONE

// System include(s):
#include <string>
#include <iosfwd>

// Local include(s):
#include "AsgMessaging/StatusCode.h"
#include "AsgServices/ServiceStore.h"

/// Non-templated base class for the ServiceHandle types
///
/// This base class takes care of implementing all the functionality of
/// ServiceHandle that doesn't depend on the template parameter of the class.
/// Making the built binaries a bit smaller, and faster.
///
/// Based on \ref ToolHandleBase

class ServiceHandleBase
{
public:
   /// Constructor with a type/name string
  ServiceHandleBase( const std::string& typeAndName, const std::string& parentName );

   /// Return the type/name string specified by the user
   const std::string& typeAndName() const;
   /// Return the service's type name
   const std::string& type() const;
   /// Return the service's instance name
   const std::string& name() const;
   /// Return the service's parent service name
   const std::string& parentName() const;

   /// Change the handle's type and name
   void setTypeAndName(const std::string& typeAndName);
   /// Change the handle's name
   void setName(const std::string& name);

protected:
   /// The type/name string specified in the constructor
   std::string m_typeAndName;
   /// The type name of the target service
   std::string m_type;
   /// The instance name of the target service
   std::string m_name;
   /// The parent service
   std::string m_parentName;

}; // class ServiceHandleBase

/// Partial re-implementation of Gaudi's ServiceHandle class
///
/// This class can be used in a dual-use service to find other
/// dual-use services that have been instantiated by the user.
///
/// Based on \ref ToolHandle implementation

template< class T >
class ServiceHandle : public ServiceHandleBase
{
public:
   /// A convenience type declaration
   typedef T value_type;

   /// Constructor from a service pointer
   ServiceHandle( const std::shared_ptr<T>& pservice,
                  const std::string& parentName );

   /// Constructor from a service name.
   ServiceHandle( const std::string& servicename, const std::string& parentName);

   /// Constructor declaring a property
   template<typename T2>
   ServiceHandle (T2 *parent, const std::string& propertyName,
               const std::string& serviceName,
               const std::string& propertyTitle = "");

   /// Dereferencing operator
   T& operator*();
   /// Dereferencing operator
   const T& operator*() const;
   /// Dereferencing operator
   T* operator->();
   /// Dereferencing operator
   const T* operator->() const;

   /// Retrieve service.
   /// For compatibility with Gaudi.
   /// Returns success if pointer is non-null and of the correct type.
   StatusCode retrieve() const;

   /// Clear out the service
   void disable () noexcept;

   /// Return true if service has no pointer or name
   bool empty() const;

   /// Return if the pointer is set
   bool isSet() const;

private:
   /// Pointer to the service
   mutable std::shared_ptr<T> m_pservice;

}; // class ServiceHandle

/// A convenience operator for printing the values of such objects
template< class T >
std::ostream& operator<< ( std::ostream& out,
                           const ServiceHandle< T >& handle );

// Include the implementation of the code:
#include "AsgServices/ServiceHandle.icc"

#endif // not XAOD_STANDALONE
#endif // ASGSERVICES_SERVICEHANDLE_H
