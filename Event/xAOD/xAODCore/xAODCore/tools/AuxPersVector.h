// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AuxPersVector.h 793737 2017-01-24 20:11:10Z ssnyder $
#ifndef XAODCORE_AUXPERSVECTOR_H
#define XAODCORE_AUXPERSVECTOR_H

// EDM include(s):
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/tools/AuxTypeVector.h"
#include "AthContainersInterfaces/IAuxSetOption.h"

namespace xAOD {

   /// Class managing concrete vector variables
   ///
   /// This class is used internally by the "special" auxiliary store
   /// objects to manage the auxiliary variables handled by them. User
   /// code probably doesn't want to touch it directly...
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 793737 $
   /// $Date: 2017-01-24 21:11:10 +0100 (Tue, 24 Jan 2017) $
   ///
   template< class T, class VEC=std::vector< T > >
   class AuxPersVector : public SG::AuxTypeVectorHolder<T, VEC> {

   public:
      /// Convenience type definition
      typedef VEC& vector_type;

      /// Constructor
      AuxPersVector( vector_type vec )
        : SG::AuxTypeVectorHolder<T, VEC> (&vec, false) {}

      virtual SG::IAuxTypeVector* clone() const {
        return new AuxPersVector<T, VEC>(*this);
      }

   }; // class AuxPersVector

} // namespace xAOD

#endif // XAODCORE_AUXPERSVECTOR_H
