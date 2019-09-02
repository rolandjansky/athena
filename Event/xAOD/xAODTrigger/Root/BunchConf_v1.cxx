/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/BunchConf_v1.h"

namespace xAOD {

   BunchConf_v1::BunchConf_v1()
      : SG::AuxElement() {

   }

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BunchConf_v1, uint32_t, configID,
                                         setConfigID )

   /////////////////////////////////////////////////////////////////////////////
   //
   //          Implementation of the basic configuration accessors
   //

   AUXSTORE_OBJECT_SETTER_AND_GETTER( BunchConf_v1, std::vector< int >,
                                      collidingBCIDs, setCollidingBCIDs )

   AUXSTORE_OBJECT_SETTER_AND_GETTER( BunchConf_v1, std::vector< float >,
                                      intensitiesBeam1, setIntensitiesBeam1 )

   bool BunchConf_v1::intensitiesBeam1Available() const {

      static const Accessor< std::vector< float > > acc( "intensitiesBeam1" );
      return acc.isAvailable( *this );
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( BunchConf_v1, std::vector< float >,
                                      intensitiesBeam2, setIntensitiesBeam2 )

   bool BunchConf_v1::intensitiesBeam2Available() const {

      static const Accessor< std::vector< float > > acc( "intensitiesBeam2" );
      return acc.isAvailable( *this );
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //        Implementation of the detailed configuration accessors
   //

   AUXSTORE_OBJECT_SETTER_AND_GETTER( BunchConf_v1, std::vector< int >,
                                      unpairedBCIDsBeam1,
                                      setUnpairedBCIDsBeam1 )

   bool BunchConf_v1::unpairedBCIDsBeam1Available() const {

      static const Accessor< std::vector< int > > acc( "unpairedBCIDsBeam1" );
      return acc.isAvailable( *this );
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( BunchConf_v1, std::vector< int >,
                                      unpairedBCIDsBeam2,
                                      setUnpairedBCIDsBeam2 )

   bool BunchConf_v1::unpairedBCIDsBeam2Available() const {

      static const Accessor< std::vector< int > > acc( "unpairedBCIDsBeam2" );
      return acc.isAvailable( *this );
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( BunchConf_v1, std::vector< float >,
                                      unpairedIntensisitesBeam1,
                                      setUnpairedIntensitiesBeam1 )

   bool BunchConf_v1::unpairedIntensitiesBeam1Available() const {

      static const Accessor< std::vector< float > >
         acc( "unpairedIntensisitesBeam1" );
      return acc.isAvailable( *this );
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( BunchConf_v1, std::vector< float >,
                                      unpairedIntensisitesBeam2,
                                      setUnpairedIntensitiesBeam2 )

   bool BunchConf_v1::unpairedIntensitiesBeam2Available() const {

      static const Accessor< std::vector< float > >
         acc( "unpairedIntensisitesBeam2" );
      return acc.isAvailable( *this );
   }

   //
   /////////////////////////////////////////////////////////////////////////////

} // namespace xAOD
