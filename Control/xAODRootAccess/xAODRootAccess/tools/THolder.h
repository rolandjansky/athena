// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: THolder.h 725531 2016-02-22 16:14:25Z krasznaa $
#ifndef XAODROOTACCESS_TOOLS_THOLDER_H
#define XAODROOTACCESS_TOOLS_THOLDER_H

// STL include(s):
#include <map>

// ROOT include(s):
#include <Rtypes.h>

// Forward declaration(s):
namespace std {
   class type_info;
}
class TClass;

namespace xAOD {

   /// @short This class takes care of holding EDM objects in memory
   ///
   /// In order to be able to access EDM objects easily as their
   /// concrete type, or as one of their base classes, a little
   /// trickery is needed.
   ///
   /// We always access the branches of the input TTree as their
   /// concrete, transient types. It is then up to our own code
   /// to decide if the user is requesting the object as a type
   /// that it can be cast to, or not.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 725531 $
   /// $Date: 2016-02-22 17:14:25 +0100 (Mon, 22 Feb 2016) $
   ///
   class THolder {

   public:
      /// Default constructor
      THolder();
      /// Constructor pointing to an object in memory 
      THolder( void* object, ::TClass* type, ::Bool_t owner = kTRUE );
      /// Constructor pointing to an object in memory
      THolder( void* object, const std::type_info& type,
               ::Bool_t owner = kTRUE );
      /// Copy constructor
      THolder( const THolder& parent );
      /// Move constructor
      THolder( THolder&& parent );
      /// The destructor cleans out the memory used by the managed object
      virtual ~THolder();

      /// Assignment operator
      THolder& operator=( const THolder& rhs );
      /// Move operator
      THolder& operator=( THolder&& rhs );

      /// Return a typeless pointer to the held object
      void* get() const;
      /// Return a typeless pointer to the held object's pointer
      void** getPtr();

      /// Replace the managed object
      virtual void set( void* obj );

      /// Check whether the holder owns its object
      ::Bool_t isOwner() const;
      /// Set whether the holder should own its object
      void setOwner( ::Bool_t state = kTRUE );

      /// Return the object as a specific pointer
      virtual void* getAs( const std::type_info& tid,
                           ::Bool_t silent = kFALSE ) const;
      /// Return the object as a specific, constant pointer
      virtual const void* getAsConst( const std::type_info& tid,
                                      ::Bool_t silent = kFALSE ) const;

      /// Return the concrete type of the object
      ::TClass* getClass() const;
      /// Return the concrete type of the object
      const std::type_info* getTypeInfo() const;

      /// Renew the object in memory
      void renew();

      /// Cache whether the type is a DataVector or an AuxElement.
      /// Allows removing repeated calls to TClass::InheritsFrom()
      /// in TEvent::setAuxStore().
      enum TypeKind {
        OTHER,
        DATAVECTOR,
        AUXELEMENT
      };
      TypeKind typeKind() const { return m_typeKind; }


   private:
      static TypeKind setTypeKind (const TClass* type);
      TypeKind m_typeKind;

   protected:
      /// Internal function used to delete the managed object from memory
      void deleteObject();

      /// Typeless pointer to the object in memory
      void* m_object;
      /// Concrete type of the object being held on to
      ::TClass* m_type;
      /// Concrete type of the object, if it doesn't have a dictionary
      const std::type_info* m_typeInfo;
      /// A flag for whether the object owns what it points to
      ::Bool_t m_owner;

   }; // class THolder

} // namespace xAOD

#endif // XAODROOTACCESS_TOOLS_THOLDER_H
