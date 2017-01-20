// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AuxPersVector.h 793264 2017-01-20 18:52:30Z ssnyder $
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
   /// $Revision: 793264 $
   /// $Date: 2017-01-20 19:52:30 +0100 (Fri, 20 Jan 2017) $
   ///
   template< class T, class VEC=std::vector< T > >
   class AuxPersVector : public SG::IAuxTypeVector {

   public:
      /// Convenience type definition
      typedef VEC& vector_type;

      /// Constructor
      AuxPersVector( vector_type vec ) : m_vec( vec ) {}

      virtual SG::IAuxTypeVector* clone() const {
        return new AuxPersVector<T, VEC>(*this);
      }

      virtual void* toPtr() {
         if (m_vec.empty())
           return 0;
         return m_vec.data();
      }
      virtual void* toVector() {
         return &m_vec;
      }
      virtual size_t size() const {
         return m_vec.size();
      }
      virtual bool resize( size_t sz ) {
         const void* orig = toPtr();
         m_vec.resize( sz );
         return toPtr() == orig;
      }
      virtual void reserve( size_t sz ) {
         m_vec.reserve( sz );
      }
      virtual void shift( size_t pos, ptrdiff_t offs ) {
         if (offs < 0) {
            if (-offs > static_cast<off_t>(pos)) offs = -pos;
            std::copy (m_vec.begin()+pos, m_vec.end(), m_vec.begin()+pos+offs);
            m_vec.resize (m_vec.size() + offs);
         }
         else if (offs > 0) {
            size_t oldsz = m_vec.size();
            m_vec.resize (m_vec.size() + offs);
            std::copy (m_vec.rbegin()+offs, m_vec.rbegin()+offs+oldsz-pos,
                       m_vec.rbegin());
            std::fill (m_vec.begin()+pos, m_vec.begin()+pos+offs, T());
         }
      }

      virtual bool setOption (const SG::AuxDataOption& option) {
         // Need to instantiate different functions depending on whether or not
         // the payload implements @c SG::IAuxSetOption.
         // From AuxTypeVector.icc
         return DataModel_detail::setOptionHelper
            (&m_vec,
             option,
             typename std::is_base_of<SG::IAuxSetOption,vector_type>::type());
      }

     virtual const std::type_info* objType() const {
        return &typeid( VEC );
     }

   private:
      /// Reference to the vector being handled
      vector_type m_vec;

   }; // class AuxPersVector

} // namespace xAOD

#endif // XAODCORE_AUXPERSVECTOR_H
