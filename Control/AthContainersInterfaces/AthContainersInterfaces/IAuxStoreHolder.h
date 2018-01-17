// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IAuxStoreHolder.h 570883 2013-11-16 18:47:55Z krasznaa $
#ifndef ATHCONTAINERSINTERFACES_IAUXSTOREHOLDER_H
#define ATHCONTAINERSINTERFACES_IAUXSTOREHOLDER_H

namespace SG {

   // Forward declaration(s):
   class IAuxStore;

   /**
    *  @short Interface for objects taking part in direct ROOT I/O
    *
    *         This interface needs to be implemented by classes that serve as
    *         auxiliary containers that we want to use in our EDM directly.
    *         It allows the I/O infrastructure to give a store object to the
    *         object implementing this interface. At that point the object given
    *         to this interface will be the one doing the ROOT I/O for the
    *         "dynamic" auxiliary variables.
    *
    *         Currently there's a single implementation of this,
    *         "xAODCore/AuxContainerBase.h", but we may want to have other
    *         implementations later on as well.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    * @author Marcin Nowak         <Marcin.Nowak@cern.ch>
    *
    * $Revision: 570883 $
    * $Date: 2013-11-16 19:47:55 +0100 (Sat, 16 Nov 2013) $
    */
   class IAuxStoreHolder {

   public:
      /// Virtual destructor to make vtable happy...
      virtual ~IAuxStoreHolder() {}

      /// Give an auxiliary store object to the holder object
      ///
      /// The purpose of this function is to give an alternate store
      /// object to this object instead of the one it uses normally.
      ///
      /// @param store Pointer to an object implementing direct ROOT I/O
      ///
      virtual void setStore( IAuxStore* store ) = 0;

      /// Return the pointer to the store object currently in use
      ///
      /// There's not much use case for this function yet, but it makes the
      /// interface complete at least.
      ///
      virtual IAuxStore* getStore() = 0;
      virtual const IAuxStore* getStore() const = 0;

      /// Type of the auxiliary store
      ///
      /// In the xAOD EDM we use auxiliary store objects in some cases to
      /// describe a single object, and in most cases to describe a container
      /// of objects. This enumeration declares which type the object
      /// implementing this interface is.
      ///
      enum AuxStoreType {
         AST_ObjectStore    = 0, ///< The store describes a single object
         AST_ContainerStore = 1  ///< The store describes a container
      };

      /// Return the type of the store object
      ///
      /// @see AuxStoreType
      ///
      virtual AuxStoreType getStoreType() const = 0;

   }; // class IAuxStoreHolder

} // namespace SG

#endif // ATHCONTAINERSINTERFACES_IAUXSTOREHOLDER_H
