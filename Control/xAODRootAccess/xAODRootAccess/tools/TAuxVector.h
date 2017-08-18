// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TAuxVector.h 797214 2017-02-14 19:51:39Z ssnyder $
#ifndef XAODROOTACCESS_TOOLS_TAUXVECTOR_H
#define XAODROOTACCESS_TOOLS_TAUXVECTOR_H

// EDM include(s):
#include "AthContainersInterfaces/IAuxTypeVector.h"
#include "AthContainersInterfaces/IAuxTypeVectorFactory.h"

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
   /// $Revision: 797214 $
   /// $Date: 2017-02-14 20:51:39 +0100 (Tue, 14 Feb 2017) $
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
      SG::IAuxTypeVector* clone() const override;

      /// Return a pointer to the start of the vector's data
      void* toPtr() override ;
      /// Return a pointer to the STL vector itself
      void* toVector() override;

      /// Return the size of the vector
      size_t size() const override;

      /// Change the size of the vector
      bool resize( size_t sz ) override;
      /// Change the capacity of the vector
      void reserve( size_t sz ) override;
      /// Shift the elements of the vector
      void shift( size_t pos, ptrdiff_t offs ) override;
      /// Insert a range of elements via move.
      bool insertMove (size_t pos, void* beg, void* end) override;

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
