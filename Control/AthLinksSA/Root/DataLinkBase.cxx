// $Id: DataLinkBase.cxx 602641 2014-06-19 13:05:08Z krasznaa $

// Local include(s):
#include "AthLinks/DataLinkBase.h"

DataLinkBase::DataLinkBase( uint32_t key )
   : m_persKey( key ), m_isDirectIO( false ) {

}

uint32_t DataLinkBase::persKey() const {

   return m_persKey;
}

void DataLinkBase::setPersKey( uint32_t key ) {

   // Check if we're actually changing anything:
   if( m_persKey == key ) return;
   // Store the new key:
   m_persKey = key;

   // Reset the internal cache:
   toTransient();

   return;
}

bool DataLinkBase::isDefault() const {

   return ( m_persKey == 0 );
}

void DataLinkBase::clear() {

   m_persKey = 0;
   return;
}
