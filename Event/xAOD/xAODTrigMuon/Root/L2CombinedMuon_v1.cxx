/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <iostream>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigMuon/versions/L2CombinedMuon_v1.h"
#include "xAODTrigMuon/L2StandAloneMuon.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"

namespace xAOD {

   L2CombinedMuon_v1::L2CombinedMuon_v1()
      : IParticle() {

   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //             Implementation of the xAOD::IParticle functions
   //

   AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( L2CombinedMuon_v1, float, double,
                                        pt )
   AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( L2CombinedMuon_v1, float, double,
                                        eta )
   AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( L2CombinedMuon_v1, float, double,
                                        phi )

   double L2CombinedMuon_v1::m() const {

      return 105.6583715;
   }

   double L2CombinedMuon_v1::e() const {

      return genvecP4().E();
   }

   double L2CombinedMuon_v1::rapidity() const {

      return genvecP4().Rapidity();
   }

   L2CombinedMuon_v1::FourMom_t L2CombinedMuon_v1::p4() const {
     FourMom_t p4;
     p4.SetPtEtaPhiM( pt(), eta(), phi(),m()); 
     return p4;	
   }

   /// this provides a GenVector (pt, eta, phi, m)
   L2CombinedMuon_v1::GenVecFourMom_t L2CombinedMuon_v1::genvecP4() const {
     return GenVecFourMom_t(pt(), eta(), phi(), m());
   } 

   Type::ObjectType L2CombinedMuon_v1::type() const {

      return Type::L2CombinedMuon;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //             Implementation of the 4-momentum setter functions
   //

   void L2CombinedMuon_v1::setPt( float pt ) {

      static const Accessor< float > acc( "pt" );
      acc( *this ) = pt;
      return;
   }

   void L2CombinedMuon_v1::setEta( float eta ) {

      static const Accessor< float > acc( "eta" );
      acc( *this ) = eta;
      return;
   }

   void L2CombinedMuon_v1::setPhi( float phi ) {

      static const Accessor< float > acc( "phi" );
      acc( *this ) = phi;
      return;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //            Implementation of the LVL2 muon iso specific functions
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2CombinedMuon_v1, float,
                                         charge, setCharge )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2CombinedMuon_v1, int,
                                         strategy, setStrategy )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2CombinedMuon_v1, int,
                                         errorFlag, setErrorFlag )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2CombinedMuon_v1, int,
                                         matchFlag, setMatchFlag )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2CombinedMuon_v1, float,
                                         sigmaPt, setSigmaPt )

   AUXSTORE_OBJECT_SETTER_AND_GETTER( L2CombinedMuon_v1,
                                      ElementLink< TrackParticleContainer >,
                                      idTrackLink, setIdTrackLink )

   const xAOD::TrackParticle* L2CombinedMuon_v1::idTrack() const {

      static const Accessor< ElementLink< TrackParticleContainer > >
         acc( "idTrackLink" );
      if( ! acc.isAvailable( *this ) ) {
         return nullptr;
      }
      const ElementLink< TrackParticleContainer >& el = acc( *this );
      if( ! el.isValid() ) {
         return nullptr;
      }
      return *el;
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( L2CombinedMuon_v1,
                                      ElementLink< L2StandAloneMuonContainer >,
                                      muSATrackLink, setMuSATrackLink )

   const xAOD::L2StandAloneMuon* L2CombinedMuon_v1::muSATrack() const {

      static const Accessor< ElementLink< L2StandAloneMuonContainer > >
         acc( "muSATrackLink" );
      if( ! acc.isAvailable( *this ) ) {
         return nullptr;
      }
      const ElementLink< L2StandAloneMuonContainer >& el = acc( *this );
      if( ! el.isValid() ) {
         return nullptr;
      }
      return *el;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

/// Helper operator for easily printing the properties of a LVL2 muon iso
/// object for debugging.
///
std::ostream& operator<< ( std::ostream& out,
                           const xAOD::L2CombinedMuon_v1& mu ) {

   out << "charge: "        << mu.charge()        << "; ";
   out << "pt: "            << mu.pt()            << "; ";
   out << "phi: "           << mu.phi()           << "; ";
   out << "eta: "           << mu.eta()           << "; ";
   out << "strategy: "      << mu.strategy()      << "; ";
   out << "errorFlag: "     << mu.errorFlag()     << "; ";
   out << "matchFlag: "     << mu.matchFlag()     << "; ";
   out << "sigmaPt: "       << mu.sigmaPt();

   // Return the stream:
   return out;
}


} // namespace xAOD
