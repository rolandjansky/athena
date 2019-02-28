/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigElectron_v1.cxx 797628 2017-02-16 21:31:00Z rwhite $

// System include(s):
#include <cmath>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigEgamma/versions/TrigElectron_v1.h"

namespace xAOD {

   /// The default constructor doesn't do much, it just sets up the object
   /// in a reasonable state. Remember that an object created with this
   /// constructor is not usable yet. You either have to add it to a container,
   /// or give it a private auxiliary store before setting properties on it.
   ///
   TrigElectron_v1::TrigElectron_v1()
      : IParticle() {

   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //       Implementation of the functions inherited from IParticle
   //

   AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TrigElectron_v1, float, double, pt )

   /// This function gets the energy of the electron using the internal
   /// TLorentzVector. Which calculates the energy based on the eta, phi, pt,
   /// and m properties.
   ///
   /// @returns The energy of the electron
   ///
   double TrigElectron_v1::e() const {

      return genvecP4().E();
   }

   /// This function gets the rapidity of the electron using the internal
   /// TLorentzVector. Which calculates the rapidity based on the eta, phi, pt,
   /// and m properties.
   ///
   /// @returns The rapidity (y) of the electron
   ///
   double TrigElectron_v1::rapidity() const {

      return genvecP4().Rapidity();
   }

   /// This function provides the 4-momentum of
   /// the object as a TLorentzVector. 
   ///
   /// @returns The full 4-momentum of the electron
   ///
   TrigElectron_v1::FourMom_t TrigElectron_v1::p4() const {
     FourMom_t p4;
     p4.SetPtEtaPhiM( pt(), eta(), phi(),m()); 
     return p4;	
   }

   /// this provides a GenVector (pt, eta, phi, m)
   TrigElectron_v1::GenVecFourMom_t TrigElectron_v1::genvecP4() const {
     return GenVecFourMom_t(pt(), eta(), phi(), m());
   } 

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //         Implementation of the "simple" electron properties
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigElectron_v1, uint32_t, roiWord,
                                         setRoiWord )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigElectron_v1, float, zvtx, setZvtx )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigElectron_v1, float, charge,
                                         setCharge )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigElectron_v1, int, nTRTHits,
                                         setNTRTHits )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigElectron_v1, int,
                                         nTRTHiThresholdHits,
                                         setNTRTHiThresholdHits )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigElectron_v1, float, rcore,
                                         setRcore )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigElectron_v1, float, eratio,
                                         setEratio )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigElectron_v1, float, etHad,
                                         setEtHad )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigElectron_v1, float, etHad1,
                                         setEtHad1 )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigElectron_v1, float, f0, setF0 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigElectron_v1, float, f1, setF1 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigElectron_v1, float, f2, setF2 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigElectron_v1, float, f3, setF3 )

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //          Implementation of the 4-momentum handling functions
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigElectron_v1, float, trkEtaAtCalo,
                                         setTrkEtaAtCalo )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigElectron_v1, float, trkPhiAtCalo,
                                         setTrkPhiAtCalo )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigElectron_v1, float, etOverPt,
                                         setEtOverPt )

   float TrigElectron_v1::trkClusDeta() const {

      return std::abs( trkEtaAtCalo() - caloEta() );
   }

   float TrigElectron_v1::trkClusDphi() const {

      float result = std::abs( trkPhiAtCalo() - caloPhi() );
      if( ! ( result < M_PI ) ) {
         result = 2 * M_PI - result;
      }
      return result;
   }

   AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( TrigElectron_v1, float, double, pt,
                                        setPt )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigElectron_v1, float, caloEta,
                                         setCaloEta ) 
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigElectron_v1, float, caloPhi,
                                         setCaloPhi )

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //          Implementation of the constituent accessor functions
   //

   /// This function can be used to conveniently access the EM cluster
   /// associated with the electron. It will return a null pointer if the
   /// associated object can't be retrieved, but it will never fail in any
   /// other way.
   ///
   /// @returns A pointer to the associated EM cluster, or a null pointer
   ///          if the object can't be found
   ///
   const TrigEMCluster* TrigElectron_v1::emCluster() const {

      // The link accessor:
      static const Accessor< EMClusterLink_t > acc( "emClusterLink" );

      // Check if the link is available:
      if( ! acc.isAvailable( *this ) ) {
         return 0;
      }

      // Check if the link is valid:
      const EMClusterLink_t& link = acc( *this );
      if( ! link.isValid() ) {
         return 0;
      }

      // Return the pointer:
      return *link;
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigElectron_v1,
                                      TrigElectron_v1::EMClusterLink_t,
                                      emClusterLink,
                                      setEmClusterLink )

   /// This function can be used to conveniently access the track particle
   /// associated with the electron. It will return a null pointer if the
   /// associated object can't be retrieved, but it will never fail in any
   /// other way.
   ///
   /// @returns A pointer to the associated track particle, or a null pointer
   ///          if the object can't be found
   ///
   const TrackParticle* TrigElectron_v1::trackParticle() const {

      // The link accessor:
      static const Accessor< TrackParticleLink_t > acc( "trackParticleLink" );

      // Check if the link is available:
      if( ! acc.isAvailable( *this ) ) {
         return 0;
      }

      // Check if the link is valid:
      const TrackParticleLink_t& link = acc( *this );
      if( ! link.isValid() ) {
         return 0;
      }

      // Return the pointer:
      return *link;
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigElectron_v1,
                                      TrigElectron_v1::TrackParticleLink_t,
                                      trackParticleLink,
                                      setTrackParticleLink )

   //
   /////////////////////////////////////////////////////////////////////////////

   /// @param roi The 32-bit RoI word
   /// @param trkEtaAtCalo The extrapolated pseudorapidity of the track at the
   ///                     calorimeter
   /// @param trkPhiAtCalo The extrapolated azimuth angle of the track at the
   ///                     calorimeter
   /// @param etOverPt The \f$E_T\f$ measured in the calorimeter over the
   ///                 \f$p_T\f$ measured in the inner detector
   /// @param clLink Link to the EM cluster constituent
   /// @param tpLink Link to the track particle constituent
   ///
   void TrigElectron_v1::init( uint32_t roi,
                               float trkEtaAtCalo, float trkPhiAtCalo,
                               float etOverPt,
                               const EMClusterLink_t& clLink,
                               const TrackParticleLink_t& tpLink ) {

      setRoiWord( roi );
      setZvtx( 0 );
      setCharge( 0 );
      setNTRTHits( 0 );
      setNTRTHiThresholdHits( 0 );
      setTrkEtaAtCalo( trkEtaAtCalo );
      setTrkPhiAtCalo( trkPhiAtCalo );
      setEtOverPt( etOverPt );
      setCaloEta( 0 );
      setCaloPhi( 0 );
      setEtHad( 0 );
      setEtHad1( 0 );
      setRcore( 0 );
      setEratio( 0 );
      setF0( 0 );
      setF1( 0 );
      setF2( 0 );
      setF3( 0 );

      // Links to associated trigger track and EM cluster.
      setTrackParticleLink( tpLink );
      setEmClusterLink( clLink );

      // Stash tracking variables
      if( tpLink.isValid() ) {

         // Get the pointer:
         const TrackParticle* tpPtr = *tpLink;

         // get track author and nr. trt hits from track
         // setTrackAlgo(*tr_ptr->algorithmId());        ----> this is clearly wrong, but I still don't know about ELs...
         // All of this code needs to be updated to modern interface one-at-a-time.
         uint8_t TRThits;
         if( tpPtr->summaryValue( TRThits, xAOD::numberOfTRTHits ) ) {
            setNTRTHits( TRThits );
         }

         uint8_t TRTThresholdhits;
         if( tpPtr->summaryValue( TRTThresholdhits,
                                  xAOD::numberOfTRTHighThresholdHits ) ) {
            setNTRTHiThresholdHits( TRTThresholdhits );
         }

         // Initialize 4-momentum base-class angular quantities from track
         // data: assume that track parameters at perigee give better
         // estimates of angular quantities near vertex
         setZvtx( tpPtr->z0() );
         setCharge( tpPtr->charge() ); // is this correct? Needs to be checked by Calo experts
      }
 
      // Stash calo cluster variables.
      if( clLink.isValid() ) {

         // Get the pointer:
         const TrigEMCluster* clPtr = *clLink;

         /* Initialize 4-momentum base-class ET from cluster data: assume
            cluster quantities give better estimate of transverse energy */
         setPt( clPtr->et() ); 
 
         // eta-phi of cluster
         setCaloEta( clPtr->eta() );
         setCaloPhi( clPtr->phi() );

         // EThad
         if( caloEta() != 0 ) {
            const float ehad1 = clPtr->ehad1();
            const float abseta = std::abs( caloEta() );
            const float cl_EThad  = ehad1 / std::cosh( abseta ); // Ehad1/cosh(fabs(eta) */
            setEtHad( cl_EThad );
            setEtHad1( cl_EThad );
         } else {
            setEtHad( clPtr->ehad1() );                      // Ehad (=EThad1)
            setEtHad1( clPtr->ehad1() );                      // Ehad1 (=EThad)
         }
 
         // Rcore (test for positive energy to avoid zeros)
         const float e277 = clPtr->e277();
         const float e237 = clPtr->e237();
         if( e277 != 0 ) { // (VD) before it was '>'
            setRcore( e237 / e277 );
         } else {
            setRcore( 0 );
         }

         // Eratio (test for positive energy to avoid zeros)
         const float emaxs1 = clPtr->emaxs1();
         const float e2tsts1 = clPtr->e2tsts1();
         if( emaxs1 + e2tsts1 > 0 ) {
            const float cl_Eratio = ( ( emaxs1 - e2tsts1 ) /
                                      ( emaxs1 + e2tsts1 ) );
            setEratio( cl_Eratio );
         } else {
            setEratio( 0 );
         }
 
         // energy and energy fraction per sample
         const float tmp_cl_energy = clPtr->energy();

         if( std::abs( tmp_cl_energy ) > 0.00001 ) { // avoid floating-point exceptions

            const float cl_e_frac_S0 =
               ( clPtr->energy( CaloSampling::PreSamplerB ) +
                 clPtr->energy( CaloSampling::PreSamplerE ) ) / tmp_cl_energy;
            setF0( cl_e_frac_S0 );

            const float cl_e_frac_S1 =
               ( clPtr->energy( CaloSampling::EMB1 ) +
                 clPtr->energy( CaloSampling::EME1 ) ) / tmp_cl_energy;
            setF1( cl_e_frac_S1 );

            const float cl_e_frac_S2 =
               ( clPtr->energy( CaloSampling::EMB2 ) +
                 clPtr->energy( CaloSampling::EME2 ) ) / tmp_cl_energy;
            setF2( cl_e_frac_S2 );

            const float cl_e_frac_S3 =
               ( clPtr->energy( CaloSampling::EMB3 ) +
                 clPtr->energy( CaloSampling::EME3 ) ) / tmp_cl_energy;
            setF3( cl_e_frac_S3 );

         } else {
            setF0(0);
            setF1(0);
            setF2(0);
            setF3(0);
         }

      }

      return;
   }

} // namespace xAOD
