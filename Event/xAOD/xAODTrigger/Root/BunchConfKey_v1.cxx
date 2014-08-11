/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchConfKey_v1.cxx 583374 2014-02-14 15:52:54Z krasznaa $

// Local include(s):
#include "xAODTrigger/versions/BunchConfKey_v1.h"

namespace xAOD {

   BunchConfKey_v1::BunchConfKey_v1( uint32_t id )
      : m_id( id ) {

   }

   uint32_t BunchConfKey_v1::id() const {

      return m_id;
   }

   void BunchConfKey_v1::setId( uint32_t id ) {

      m_id = id;
      return;
   }

} // namespace xAOD
