// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TAuxVector.h 793319 2017-01-21 16:21:46Z ssnyder $
#ifndef XAODROOTACCESS_TOOLS_TAUXVECTOR_H
#define XAODROOTACCESS_TOOLS_TAUXVECTOR_H

// EDM include(s):
#include "AthContainersInterfaces/IAuxTypeVector.h"

// Forward declaration(s):
class TClass;
class TVirtualCollectionProxy;
namespace SG {
   class IAuxTypeVectorFactory;
}

namespace xAOD {

   /// Auxiliary vector type for types known to ROOT
   ///
   /// This class is used for types known to ROOT, which have not received a
   /// concrete auxiliary vector type yet. (By having been accessed explicitly.)
   ///
   /// The code is pretty much a copy of what Scott wrote for RootStorageSvc for
   /// the offline code.
   ///
   /// @author Scott Snyder <Scott.Snyder@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 793319 $
   /// $Date: 2017-01-21 17:21:46 +0100 (Sat, 21 Jan 2017) $
   ///
   class TAuxVector : public SG::IAuxTypeVector {

   public:
      /// Constructor
      TAuxVector( const SG::IAuxTypeVectorFactory* factory,
                  ::TClass* cl, size_t size, size_t capacity );
      /// Copy constructor
      TAuxVector( const TAuxVector& parent );
      /// Destructor
      ~TAuxVector();

      /// Assignment operator
      TAuxVector& operator= ( const TAuxVector& other );

      /// @name Function implementing the SG::IAuxTypeVector interface
      /// @{

      /// Copy the managed vector
      virtual SG::IAuxTypeVector* clone() const;

      /// Return a pointer to the start of the vector's data
      virtual void* toPtr();
      /// Return a pointer to the STL vector itself
      virtual void* toVector();

      /// Return the size of the vector
      virtual size_t size() const;

      /// Change the size of the vector
      virtual bool resize( size_t sz );
      /// Change the capacity of the vector
      virtual void reserve( size_t sz );
      /// Shift the elements of the vector
      virtual void shift( size_t pos, ptrdiff_t offs );

      /// @}

   private:
      /// Function copying the payload of a range to a new location
      void copyRange( const void* src, void* dst, size_t n );
      // Function clearing the payload of a given range
      void clearRange( void* dst, size_t n );

      /// The parent factory object
      const SG::IAuxTypeVectorFactory* m_factory;
      /// ROOT's description of the vector type
      ::TVirtualCollectionProxy* m_proxy;
      /// Pointer to the vector object
      void* m_vec;

   }; // class TAuxVector

} // namespace xAOD

#endif // XAODROOTACCESS_TOOLS_TAUXVECTOR_H
