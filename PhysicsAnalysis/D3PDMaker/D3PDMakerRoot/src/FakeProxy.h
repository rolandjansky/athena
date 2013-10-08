// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FakeProxy.h 487276 2012-03-08 11:04:56Z krasznaa $
#ifndef D3PDMAKERROOT_FAKEPROXY_H
#define D3PDMAKERROOT_FAKEPROXY_H

// ROOT include(s):
#include <TMethodCall.h>

// Forward declaration(s):
class TClass;

namespace D3PD {

   /**
    *  @short Proxy class for storing any kind of object
    *
    *         This proxy is used to create and take ownership of object
    *         types that will not be written out to the D3PD file, but the
    *         block filler tools still need them to exist in memory.
    *
    *         The class can handle both primitive and class types.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 487276 $
    * $Date: 2012-03-08 12:04:56 +0100 (Thu, 08 Mar 2012) $
    */
   class FakeProxy {

   public:
      /// Constructor taking ownership of an object
      FakeProxy( void* object, const std::type_info& ti );
      /// Destructor deleting the encapsulated object
      ~FakeProxy();

      /// Clear the contents of this variable
      void clear();

      /// Access the dictionary of a specific object
      static ::TClass* getClass( const std::type_info& ti );
      /// Create a new instance of a primitive
      static void* newPrimitive( const std::type_info& ti );
      /// Delete a primitive
      static void deletePrimitive( void* ptr, const std::type_info& ti );

   private:
      /// Variable clearing method
      /**
       * Primitive types are just zeroed out, while for class types
       * the code checks if they have a <code>clear()</code> function.
       * If they do, it will be used to clear the variable.
       */
      enum ClearType {
         UNKNOWN, ///< Can't clear the variable
         ZERO,    ///< Clear the variable by filling it with zeroes
         CLEAR    ///< Clear the variable by calling clear() on it
      };
      ClearType m_clearType; ///< The variable clearing type for this object

      void* m_object; ///< The owned object
      const std::type_info& m_type; ///< The type of the owned object
      ::TMethodCall m_clear; ///< Method calling clear on the object
      ::TClass* m_dict; ///< Dictionary for class types

   }; // class FakeProxy

} // namespace D3PD

#endif // D3PDMAKERROOT_FAKEPROXY_H
