/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EnergySumRoI_v2.cxx 711888 2015-12-01 13:36:40Z watsona $

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/EnergySumRoI_v2.h"

namespace xAOD {

   EnergySumRoI_v2::EnergySumRoI_v2()
      : SG::AuxElement() {
   }

   /// @param roiword0 The first RoI word produced by the L1Calo hardware
   /// @param roiword1 The second RoI word produced by the L1Calo hardware
   /// @param roiword2 The third RoI word produced by the L1Calo hardware
   /// @param energyX The total energy deposited in the X direction
   /// @param energyY The total energy deposited in the Y direction
   /// @param energyT The total transverse energy deposited
   ///
   void EnergySumRoI_v2::initialize( uint32_t roiword0, uint32_t roiword1,
                                     uint32_t roiword2,
                                     float energyX, float energyY,
                                     float energyT ) {

      bool restricted = ((roiword0&s_Run2TruncRoIBit) != 0);      
      
      if (restricted) {
        setRoIWord3( roiword0 );
        setRoIWord4( roiword1 );
        setRoIWord5( roiword2 );
        setEnergyXRestricted( energyX );
        setEnergyYRestricted( energyY );
        setEnergyTRestricted( energyT );
      }
      else {
        setRoIWord0( roiword0 );
        setRoIWord1( roiword1 );
        setRoIWord2( roiword2 );
        setEnergyX( energyX );
        setEnergyY( energyY );
        setEnergyT( energyT );
      }
   
      return;
   }

   /// Update RoI word value. The function works out for itself which word input is

   void EnergySumRoI_v2::setRoIWord( uint32_t roiword ) {

      bool word0 = ((roiword&s_RoIType) == s_Word0);
      bool word1 = ((roiword&s_RoIType) == s_Word1);
      bool word2 = ((roiword&s_RoIType) == s_Word2);
      bool restricted = ((roiword&s_Run2TruncRoIBit) != 0);

      if (!restricted) {
         if      (word0) setRoIWord0(roiword);
         else if (word1) setRoIWord1(roiword);
         else if (word2) setRoIWord2(roiword);
      }
      else {
         if      (word0) setRoIWord3(roiword);
         else if (word1) setRoIWord4(roiword);
         else if (word2) setRoIWord5(roiword);
      }

      return;

   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //            Implementation for the RoI word accessor functions
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EnergySumRoI_v2, uint32_t, roiWord0,
                                         setRoIWord0 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EnergySumRoI_v2, uint32_t, roiWord1,
                                         setRoIWord1 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EnergySumRoI_v2, uint32_t, roiWord2,
                                         setRoIWord2 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EnergySumRoI_v2, uint32_t, roiWord3,
                                         setRoIWord3 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EnergySumRoI_v2, uint32_t, roiWord4,
                                         setRoIWord4 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EnergySumRoI_v2, uint32_t, roiWord5,
                                         setRoIWord5 )

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //           Implementation of the threshold pattern helper functions
   //

   uint32_t EnergySumRoI_v2::thrPatSumET(bool restricted) const {

      if (restricted) return ( ( roiWord3() >> 16 ) & 0xff ); 
      else            return ( ( roiWord0() >> 16 ) & 0xff );
   }

   uint32_t EnergySumRoI_v2::thrPatMET(bool restricted) const {

      if (restricted) return ( ( roiWord4() >> 16 ) & 0xff ); 
      else            return ( ( roiWord1() >> 16 ) & 0xff );
   }

   uint32_t EnergySumRoI_v2::thrPatMETSig() const {

      return ( ( roiWord2() >> 16 ) & 0xff );
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //               Implementation of the overflow helper functions
   //

   bool EnergySumRoI_v2::overflowX(bool restricted) const {

      if (restricted) return ( ( roiWord3() >> 15 ) & 0x1 );
      else            return ( ( roiWord0() >> 15 ) & 0x1 );
   }

   bool EnergySumRoI_v2::overflowY(bool restricted) const {

      if (restricted) return ( ( roiWord4() >> 15 ) & 0x1 );
      else            return ( ( roiWord1() >> 15 ) & 0x1 );
   }

   bool EnergySumRoI_v2::overflowT(bool restricted) const {

      if (restricted) return ( ( roiWord5() >> 15 ) & 0x1 );
      else            return ( ( roiWord2() >> 15 ) & 0x1 );
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

   void EnergySumRoI_v2::addThreshold( const std::string& name ) {

      names( *this ).push_back( name );
      return;
   }

   void EnergySumRoI_v2::clearThresholds() {

      names( *this ).clear();
      return;
   }

   const std::vector< std::string >& EnergySumRoI_v2::thrNames() const {

      return names( *this );
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //              Implementation for the energy accessor functions
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EnergySumRoI_v2, float, energyX,
                                         setEnergyX ) 
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EnergySumRoI_v2, float, energyY,
                                         setEnergyY ) 
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EnergySumRoI_v2, float, energyT,
                                         setEnergyT ) 
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EnergySumRoI_v2, float, energyXRestricted,
                                         setEnergyXRestricted ) 
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EnergySumRoI_v2, float, energyYRestricted,
                                         setEnergyYRestricted ) 
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EnergySumRoI_v2, float, energyTRestricted,
                                         setEnergyTRestricted ) 


   // User functions for setting and getting either full or restricted ET
   //void EnergySumRoI_v2::setEnergyX( float value, bool restricted ) {

   //   if (restricted)  setEnergyXTrunc(value);
   //   else            setEnergyXFull(value);
   //}

   //void EnergySumRoI_v2::setEnergyY( float value, bool restricted ) {

   //   if (restricted)  setEnergyYTrunc(value);
   //   else            setEnergyYFull(value);
   //}

   //void EnergySumRoI_v2::setEnergyT( float value, bool restricted )  {

   //   if (restricted)  setEnergyTTrunc(value);
   //   else            setEnergyTFull(value);
   //}

   //float EnergySumRoI_v2::energyX( bool restricted ) const {

   //   if (restricted) return energyXTrunc();
   //   else           return energyXFull();
   //}

   //float EnergySumRoI_v2::energyY( bool restricted ) const {

   //   if (restricted) return energyYTrunc();
   //   else           return energyYFull();
   //}

   //float EnergySumRoI_v2::energyT( bool restricted ) const {

   //   if (restricted) return energyTTrunc();
   //   else           return energyTFull();
   //}

   //
   /////////////////////////////////////////////////////////////////////////////

} // namespace xAOD
