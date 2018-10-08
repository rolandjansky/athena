/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L2IsoMuon_v1.cxx 605614 2014-07-09 12:49:31Z krasznaa $

// System include(s):
#include <iostream>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigMuon/versions/L2IsoMuon_v1.h"

namespace xAOD {

   L2IsoMuon_v1::L2IsoMuon_v1()
      : IParticle(), m_p4(), m_p4Cached( false ) {

   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //             Implementation of the xAOD::IParticle functions
   //

   AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( L2IsoMuon_v1, float, double,
                                        pt )
   AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( L2IsoMuon_v1, float, double,
                                        eta )
   AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( L2IsoMuon_v1, float, double,
                                        phi )

   double L2IsoMuon_v1::m() const {

      return 105.6583715;
   }

   double L2IsoMuon_v1::e() const {

      return p4().E();
   }

   double L2IsoMuon_v1::rapidity() const {

      return p4().Rapidity();
   }

   const L2IsoMuon_v1::FourMom_t& L2IsoMuon_v1::p4() const {

      // Update the cached object if necessary:
      if( ! m_p4Cached ) {
         m_p4Cached = true;
         m_p4.SetPtEtaPhiM( pt(), eta(), phi(), m() );
      }

      // Return the cached object:
      return m_p4;
   }

   Type::ObjectType L2IsoMuon_v1::type() const {

      return Type::L2IsoMuon;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //             Implementation of the 4-momentum setter functions
   //

   void L2IsoMuon_v1::setPt( float pt ) {

      static Accessor< float > acc( "pt" );
      acc( *this ) = pt;
      m_p4Cached = false;
      return;
   }

   void L2IsoMuon_v1::setEta( float eta ) {

      static Accessor< float > acc( "eta" );
      acc( *this ) = eta;
      m_p4Cached = false;
      return;
   }

   void L2IsoMuon_v1::setPhi( float phi ) {

      static Accessor< float > acc( "phi" );
      acc( *this ) = phi;
      m_p4Cached = false;
      return;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //            Implementation of the LVL2 muon iso specific functions
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2IsoMuon_v1, uint32_t,
                                         roiWord, setRoiWord )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2IsoMuon_v1, float,
                                         charge, setCharge )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2IsoMuon_v1, int,
                                         errorFlag, setErrorFlag )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2IsoMuon_v1, float,
                                         sumPt01, setSumPt01 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2IsoMuon_v1, float,
                                         sumPt02, setSumPt02 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2IsoMuon_v1, float,
                                         sumPt03, setSumPt03 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2IsoMuon_v1, float,
                                         sumPt04, setSumPt04 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2IsoMuon_v1, float,
                                         sumEt01, setSumEt01 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2IsoMuon_v1, float,
                                         sumEt02, setSumEt02 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2IsoMuon_v1, float,
                                         sumEt03, setSumEt03 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L2IsoMuon_v1, float,
                                         sumEt04, setSumEt04 )
   //
   /////////////////////////////////////////////////////////////////////////////

} // namespace xAOD

/// Helper operator for easily printing the properties of a LVL2 muon iso
/// object for debugging.
///
std::ostream& operator<< ( std::ostream& out,
                           const xAOD::L2IsoMuon_v1& mu ) {

   out << "roiWord: "       << mu.roiWord()       << "; ";
   out << "charge: "        << mu.charge()        << "; ";
   out << "pt: "            << mu.pt()            << "; ";
   out << "phi: "           << mu.phi()           << "; ";
   out << "eta: "           << mu.eta()           << "; ";
   out << "errorFlag: "     << mu.errorFlag()     << "; ";
   out << "sumPt01: "       << mu.sumPt01()       << "; ";
   out << "sumPt02: "       << mu.sumPt02()       << "; ";
   out << "sumPt03: "       << mu.sumPt03()       << "; ";
   out << "sumPt04: "       << mu.sumPt04()       << "; ";
   out << "sumEt01: "       << mu.sumEt01()       << "; ";
   out << "sumEt02: "       << mu.sumEt02()       << "; ";
   out << "sumEt03: "       << mu.sumEt03()       << "; ";
   out << "sumEt04: "       << mu.sumEt04();

   // Return the stream:
   return out;
}
