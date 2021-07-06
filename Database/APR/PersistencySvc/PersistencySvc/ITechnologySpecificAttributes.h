/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INCLUDE_PERSISTENCYSVC_ITECHNOLOGYSPECIFICATTRIBUTES_H
#define INCLUDE_PERSISTENCYSVC_ITECHNOLOGYSPECIFICATTRIBUTES_H

// includes
#include "PersistencySvc/PersistencySvcException.h"
#include <string>
#include <sstream>
#include <typeinfo>

namespace pool {

  /** @class ITechnologySpecificAttributes ITechnologySpecificAttributes.h PersistencySvc/ITechnologySpecificAttributes.h
   *
   *  ITechnologySpecificAttributes is the interface for an object holding technology-specific attributes
   *
   */

  class ITechnologySpecificAttributes {
  public:
    /// Exception thrown by the ITechnologySpecificAttributes class
    class Exception : public PersistencySvcException {
    public:
      /// Constructor
      Exception( const std::string& message ):
        PersistencySvcException( message,
                                 "ITechnologySpecificAttributes" )
      {}
      /// Destructor
      virtual ~Exception() throw() {}
    };

    /// Templated method to retrieve an attribute
    template< class T > T attribute( const std::string& attributeName,
                                     std::string option = "" ) {
      T data;
      const std::type_info& typeInfo = typeid(T);
      if ( ! this->attributeOfType( attributeName,
                                    static_cast< void* >( &data ),
                                    typeInfo,
                                    option ) ) {
        std::ostringstream error;
        error << "Failed to retrieve attribute " << attributeName << " of type " << typeInfo.name();
        throw Exception( error.str() );
      }
      return data;
    }

    /// Templated method to set an attribute
    template< class T > bool setAttribute( const std::string& attributeName,
                                           const T& atttibuteValue,
                                           std::string option = "" ) {
      return this->setAttributeOfType( attributeName,
                                       static_cast< const void* >( &atttibuteValue ),
                                       typeid(T),
                                       option );
    }

  protected:
    /// Default destructor
    virtual ~ITechnologySpecificAttributes() {}

    /// The actual method returning the attribute data given a name
    virtual bool attributeOfType( const std::string& attributeName,
                                  void* data,
                                  const std::type_info& typeInfo,
                                  const std::string& option ) = 0;

    /// The actual method setting the attribute data given a name
    virtual bool setAttributeOfType( const std::string& attributeName,
                                     const void* data,
                                     const std::type_info& typeInfo,
                                     const std::string& option ) = 0;
  };

}

#endif
