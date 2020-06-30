/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

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

bool ElementLinkBase::isDefaultIndex() const {
  return m_persIndex == ElementLinkBase::INVALID;
}

bool ElementLinkBase::isDefault() const {
  // I'm not sure how necessary this is, but we also check if there is a cached
  // element in case the link is referring to a transient object (I am not sure
  // if this is allowed in AnalysisBase)
  return !hasCachedElement() && isDefaultIndex();
}
