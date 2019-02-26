/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigPhoton_v1.cxx 602892 2014-06-20 12:23:53Z krasznaa $

// System include(s):
#include <cmath>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigEgamma/versions/TrigPhoton_v1.h"

namespace xAOD {

   TrigPhoton_v1::TrigPhoton_v1()
      : IParticle() {

   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //       Implementation of the functions inherited from IParticle
   //

   AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TrigPhoton_v1, float, double, pt )

   /// This function gets the energy of the electron using the internal
   /// TLorentzVector. Which calculates the energy based on the eta, phi, pt,
   /// and m properties.
   ///
   /// @returns The energy of the electron
   ///
   double TrigPhoton_v1::e() const {

      return genvecP4().E();
   }

   /// This function gets the rapidity of the electron using the internal
   /// TLorentzVector. Which calculates the rapidity based on the eta, phi, pt,
   /// and m properties.
   ///
   /// @returns The rapidity (y) of the electron
   ///
   double TrigPhoton_v1::rapidity() const {

      return genvecP4().Rapidity();
   }

   /// This function provides the 4-momentum of
   /// the object as a TLorentzVector.
   ///
   /// @returns The full 4-momentum of the electron
   ///
   TrigPhoton_v1::FourMom_t TrigPhoton_v1::p4() const {
     FourMom_t p4;
     p4.SetPtEtaPhiM( pt(), eta(), phi(),m()); 
     return p4;	
   }

   /// this provides a GenVector (pt, eta, phi, m)
   TrigPhoton_v1::GenVecFourMom_t TrigPhoton_v1::genvecP4() const {
     return GenVecFourMom_t(pt(), eta(), phi(), m());
   } 

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //         Implementation of the "simple" photon properties
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigPhoton_v1, uint32_t, roiWord,
                                         setRoiWord )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigPhoton_v1, float, rcore,
                                         setRcore )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigPhoton_v1, float, eratio,
                                         setEratio )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigPhoton_v1, float, etHad,
                                         setEtHad )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigPhoton_v1, float, etHad1,
                                         setEtHad1 )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigPhoton_v1, float, f0, setF0 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigPhoton_v1, float, f1, setF1 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigPhoton_v1, float, f2, setF2 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigPhoton_v1, float, f3, setF3 )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigPhoton_v1, float, fside,
                                         setFside )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigPhoton_v1, float, weta2,
                                         setWeta2 )

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //          Implementation of the 4-momentum handling functions
   //

   AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( TrigPhoton_v1, float, double, pt,
                                        setPt )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigPhoton_v1, float, caloEta,
                                         setCaloEta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigPhoton_v1, float, caloPhi,
                                         setCaloPhi )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigPhoton_v1, float, dEta, setDEta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigPhoton_v1, float, dPhi, setDPhi )

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
   const TrigEMCluster* TrigPhoton_v1::emCluster() const {

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

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigPhoton_v1,
                                      TrigPhoton_v1::EMClusterLink_t,
                                      emClusterLink,
                                      setEmClusterLink )

   //
   /////////////////////////////////////////////////////////////////////////////

   /// @param roi The 32-bit RoI word
   /// @param dphi Add documentation...
   /// @param deta Add documentation...
   /// @param clLink Link to the EM cluster constituent
   ///
   void TrigPhoton_v1::init( uint32_t roi,
                             float dphi, float deta,
                             const EMClusterLink_t& clLink ) {

      setRoiWord( roi );
      // Marked as oboslete in old EDM, so we drop it here?
      setCaloEta( 0 );
      setCaloPhi( 0 );
      setDEta( deta );
      setDPhi( dphi );
      setEtHad( 0 );
      setEtHad1( 0 );
      setRcore( 0 );
      setEratio( 0 );
      setF0( 0 );
      setF1( 0 );
      setF2( 0 );
      setF3( 0 );
      setFside( 0 );
      setWeta2( 0 );

      // Links to associated trigger track and EM cluster.
      setEmClusterLink( clLink );

      // Stash calo cluster variables.
      if( clLink.isValid() ) {

         // Get the pointer:
         const TrigEMCluster* clPtr = *clLink;

         setPt( clPtr->et() );
         setCaloEta( clPtr->eta() );
         setCaloPhi( clPtr->phi() );

         // EThad
         if( caloEta() != 0 ) {
            const float ehad = clPtr->ehad1();
            const float cl_EThad  = ehad / std::cosh( std::abs( caloEta() ) ); // Ehad1/cosh(fabs(eta) */
            setEtHad( cl_EThad );
            setEtHad1( cl_EThad );
         } else {
            setEtHad( clPtr->ehad1() );                      // Ehad1 (=EThad)
            setEtHad1( clPtr->ehad1() );                      // Ehad (=EThad1)
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

            setFside( clPtr->fracs1() );
            setWeta2( clPtr->weta2() );

         } else {
            setF0( 0 );
            setF1( 0 );
            setF2( 0 );
            setF3( 0 );

            setFside( 0 );
            setWeta2( 0 );
         }

      }

      return;
  }

} // namespace xAOD
