/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetEtRoI_v1.cxx 579914 2014-01-24 10:15:15Z krasznaa $

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/JetEtRoI_v1.h"

namespace xAOD {

   JetEtRoI_v1::JetEtRoI_v1()
      : SG::AuxElement() {

   }

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetEtRoI_v1, uint32_t, roiWord,
                                         setRoIWord ) 

   uint32_t JetEtRoI_v1::thrPattern() const {

      return ( roiWord() & 0xf );
   }

   /// Accessor for the names of the passed thresholds
   static SG::AuxElement::Accessor< std::vector< std::string > >
      names( "thrNames" );

   void JetEtRoI_v1::addThreshold( const std::string& name ) {

      names( *this ).push_back( name );
      return;
   }

   void JetEtRoI_v1::clearThresholds() {

      names( *this ).clear();
      return;
   }

   const std::vector< std::string >& JetEtRoI_v1::thrNames() const {

      return names( *this );
   } 

} // namespace xAOD
