// $Id: ElementLinkVectorBase.cxx 579518 2014-01-22 11:01:30Z krasznaa $

// Local include(s):
#include "AthLinks/ElementLinkVectorBase.h"

ElementLinkVectorBase::
ElementLinkVectorBase( const std::vector< uint32_t >& keys,
                       const std::vector< uint32_t >& indices )
   : m_persKeys( keys ), m_persIndices( indices ), m_isDirectIO( false ) {

}

const std::vector< uint32_t >& ElementLinkVectorBase::persKeys() const {

   return m_persKeys;
}

void
ElementLinkVectorBase::setPersKeys( const std::vector< uint32_t >& keys ) {

   m_persKeys = keys;
   return;
}

const std::vector< uint32_t >& ElementLinkVectorBase::persIndices() const {

   return m_persIndices;
}

void
ElementLinkVectorBase::
setPersIndices( const std::vector< uint32_t >& indices ) {

   m_persIndices = indices;
   return;
}
