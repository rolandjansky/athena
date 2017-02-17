// $Id: ElementLinkBase.cxx 579518 2014-01-22 11:01:30Z krasznaa $

// Local include(s):
#include "AthLinks/ElementLinkBase.h"

ElementLinkBase::ElementLinkBase( uint32_t key, uint32_t index )
   : m_persKey( key ), m_persIndex( index ), m_isDirectIO( false ) {

}

uint32_t ElementLinkBase::persKey() const {

   return m_persKey;
}

void ElementLinkBase::setPersKey( uint32_t key ) {

   // Check if we're actually changing anything:
   if( m_persKey == key ) return;
   // Store the new key:
   m_persKey = key;

   // Reset the internal cache:
   toTransient();

   return;
}

uint32_t ElementLinkBase::persIndex() const {

   return m_persIndex;
}

void ElementLinkBase::setPersIndex( uint32_t index ) {

   // Check if we're actually changing anything:
   if( m_persIndex == index ) return;
   // Store the new index:
   m_persIndex = index;

   // Reset the internal cache:
   toTransient();

   return;
}
