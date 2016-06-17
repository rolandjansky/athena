/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EnergySumRoI_v1.cxx 711888 2015-12-01 13:36:40Z watsona $

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/EnergySumRoI_v1.h"

namespace xAOD {

   EnergySumRoI_v1::EnergySumRoI_v1()
      : SG::AuxElement() {

   }

   /// @param roiword0 The first RoI word produced by the L1Calo hardware
   /// @param roiword1 The second RoI word produced by the L1Calo hardware
   /// @param roiword2 The third RoI word produced by the L1Calo hardware
   /// @param energyX The total energy deposited in the X direction
   /// @param energyY The total energy deposited in the Y direction
   /// @param energyT The total transverse energy deposited
   ///
   void EnergySumRoI_v1::initialize( uint32_t roiword0, uint32_t roiword1,
                                     uint32_t roiword2,
                                     float energyX, float energyY,
                                     float energyT ) {

      setRoIWord0( roiword0 );
      setRoIWord1( roiword1 );
      setRoIWord2( roiword2 );

      setEnergyX( energyX );
      setEnergyY( energyY );
      setEnergyT( energyT );

      return;
   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //            Implementation for the RoI word accessor functions
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EnergySumRoI_v1, uint32_t, roiWord0,
                                         setRoIWord0 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EnergySumRoI_v1, uint32_t, roiWord1,
                                         setRoIWord1 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EnergySumRoI_v1, uint32_t, roiWord2,
                                         setRoIWord2 )

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //           Implementation of the threshold pattern helper functions
   //

   uint32_t EnergySumRoI_v1::thrPatSumET() const {

      return ( ( roiWord0() >> 16 ) & 0xff );
   }

   uint32_t EnergySumRoI_v1::thrPatMET() const {

      return ( ( roiWord1() >> 16 ) & 0xff );
   }

   uint32_t EnergySumRoI_v1::thrPatMETSig() const {

      return ( ( roiWord2() >> 16 ) & 0xff );
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //               Implementation of the overflow helper functions
   //

   bool EnergySumRoI_v1::overflowX() const {

      return ( ( roiWord0() >> 15 ) & 0x1 );
   }

   bool EnergySumRoI_v1::overflowY() const {

      return ( ( roiWord1() >> 15 ) & 0x1 );
   }

   bool EnergySumRoI_v1::overflowT() const {

      return ( ( roiWord2() >> 15 ) & 0x1 );
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //            Implementation of the threshold accessor functions
   //

   /// Accessor for the names of the passed thresholds
   static SG::AuxElement::Accessor< std::vector< std::string > >
      names( "thrNames" );

   void EnergySumRoI_v1::addThreshold( const std::string& name ) {

      names( *this ).push_back( name );
      return;
   }

   void EnergySumRoI_v1::clearThresholds() {

      names( *this ).clear();
      return;
   }

   const std::vector< std::string >& EnergySumRoI_v1::thrNames() const {

      return names( *this );
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //              Implementation for the energy accessor functions
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EnergySumRoI_v1, float, energyX,
                                         setEnergyX ) 
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EnergySumRoI_v1, float, energyY,
                                         setEnergyY ) 
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EnergySumRoI_v1, float, energyT,
                                         setEnergyT ) 

   //
   /////////////////////////////////////////////////////////////////////////////

} // namespace xAOD
