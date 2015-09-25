// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AuxPersInfo.h 634134 2014-12-05 19:27:41Z ssnyder $
#ifndef XAODCORE_TOOLS_AUXPERSINFO_H
#define XAODCORE_TOOLS_AUXPERSINFO_H

// System include(s):
#include <stdexcept>

// EDM include(s):
#include "AthContainersInterfaces/IAuxTypeVector.h"

namespace xAOD {

   /// Class managing concrete persistent variables
   ///
   /// This class is used internally by the "special" auxiliary store
   /// objects to manage the auxiliary variables handled by them. User
   /// code probably doesn't want to touch it directly...
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 634134 $
   /// $Date: 2014-12-05 20:27:41 +0100 (Fri, 05 Dec 2014) $
   ///
   template< class T >
   class AuxPersInfo : public SG::IAuxTypeVector {

   public:
      /// Convenience type definition
      typedef T& info_type;

      /// Constructor
      AuxPersInfo( info_type info ) : m_info( info ) {}

      virtual SG::IAuxTypeVector* clone() const {
        return new AuxPersInfo<T>(*this);
      }

      virtual void* toPtr() {
         return &m_info;
      }
      virtual void* toVector() {
         return &m_info;
      }
      virtual size_t size() const {
         return 1;
      }
      virtual void resize( size_t sz ) {
         if( sz != 1 ) {
            throw std::runtime_error( "Calling resize with != 1 on a "
                                      "non-vector" );
         }
      }
      virtual void reserve( size_t sz ) {
         if( sz != 1 ) {
            throw std::runtime_error( "Calling reserve with != 1 on a "
                                      "non-vector" );
         }
      }
      virtual void shift( size_t /*pos*/, ptrdiff_t /*offs*/ ) {
         throw std::runtime_error( "Calling shift on a non-vector" );
      }

      virtual const std::type_info* objType() const {
         return &typeid(T);
      }

   private:
      /// Reference to the info being handled
      info_type m_info;

   }; // class AuxPersInfo

} // namespace xAOD

#endif // XAODCORE_TOOLS_AUXPERSINFO_H
