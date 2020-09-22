/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ROOT include(s):
#include <TClass.h>
#include <TError.h>

// Local include(s):
#include "xAODRootAccess/TPyStore.h"
#include "xAODRootAccess/tools/ReturnCheck.h"

namespace xAOD {

   /// This function can be used in the same manner as
   /// TStore::contains<...>(...), but instead of providing a type, one gives it
   /// a type name.
   ///
   /// Note that only types that have a proper ROOT dictionary can be used.
   /// Unlike C++, which allows one to insert any type of object into TStore.
   ///
   /// @param key  The key of the object that we're looking for
   /// @param type The type name of the object we're trying to access
   /// @returns <code>kTRUE</code> if the object is acessible,
   ///          <code>kFALSE</code> otherwise
   ///
   ::Bool_t TPyStore::contains( const std::string& key,
                                const std::string& type ) const {

      // Try to access the dictionary of this type:
      ::TClass* cl = ::TClass::GetClass( type.c_str() );
      if( ! cl ) {
         ::Warning( "xAOD::TPyStore::contains",
                    "Type name \"%s\" not known", type.c_str() );
         return kFALSE;
      }

      // Check if the dictionary can return a type_info:
      const std::type_info* ti = cl->GetTypeInfo();
      if( ! ti ) {
         ::Warning( "xAOD::TPyStore::contains",
                    "Type \"%s\" doesn't have a proper dictionary",
                    type.c_str() );
         return kFALSE;
      }

      // Use the base class to answer the question:
      return TStore::contains( key, *ti );
   }

   /// This function can be used in the same manner as
   /// TStore::isConst<...>(...), but instead of providing a type, one gives it
   /// a type name.
   ///
   /// Note that only types that have a proper ROOT dictionary can be used.
   /// Unlike C++, which allows one to insert any type of object into TStore.
   ///
   /// @param key  The key of the object that we're looking for
   /// @param type The type name of the object we're trying to access
   /// @returns <code>kTRUE</code> if the object is acessible,
   ///          <code>kFALSE</code> otherwise
   ///
   ::Bool_t TPyStore::isConst( const std::string& key,
                               const std::string& type ) const {

      // Try to access the dictionary of this type:
      ::TClass* cl = ::TClass::GetClass( type.c_str() );
      if( ! cl ) {
         ::Warning( "xAOD::TPyStore::isConst",
                    "Type name \"%s\" not known", type.c_str() );
         return kFALSE;
      }

      // Check if the dictionary can return a type_info:
      const std::type_info* ti = cl->GetTypeInfo();
      if( ! ti ) {
         ::Warning( "xAOD::TPyStore::isConst",
                    "Type \"%s\" doesn't have a proper dictionary",
                    type.c_str() );
         return kFALSE;
      }

      // Use the base class to answer the question:
      return TStore::isConst( key, *ti );
   }

   /// This function can be used in the same manner as
   /// TStore::record<...>(...), but instead of providing a type, one gives it
   /// a type name and a typeless pointer.
   ///
   /// Note that only types that have a proper ROOT dictionary can be used.
   /// Unlike C++, which allows one to insert any type of object into TStore.
   ///
   /// Also note that this function doesn't take ownership of the recorded
   /// object. In Python all the objects created by the interpreter are managed
   /// by the interpreter. So this code is not supposed to delete them,
   /// otherwise all hell breaks loose.
   ///
   /// @param obj  Pointer to the object to be put into the store
   /// @param key  Key of the object in the store
   /// @param type The type name of the object we are inserting
   /// @returns The usual xAOD::TReturnCode types
   ///
   TReturnCode TPyStore::record( void* obj, const std::string& key,
                                 const std::string& type ) {

      // Simply forward the call to the appropriate function from the base
      // class:
      RETURN_CHECK( "xAOD::TPyStore::record",
                    TStore::record( obj, key, type, kFALSE ) );

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   /// This is just a convenience function, to make it easier to print the
   /// contents of such objects in Python. Since the base class's print()
   /// function has a special meaning in Python.
   ///
   void TPyStore::dump() const {

      print();
      return;
   }

} // namespace xAOD
