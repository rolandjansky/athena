/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthEvent_v1.cxx 761798 2016-07-14 08:15:01Z krasznaa $

// System include(s):
#include <cmath>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTruth/versions/TruthEvent_v1.h"
#include "TruthAccessors_v1.h"

namespace xAOD {


   TruthEvent_v1::TruthEvent_v1()
      : TruthEventBase_v1() {

   }

   /////////////////////////////////////////////////////////////////////////////
   // Simple, always-present event properties

   /// @todo Need upgrade to allow string-valued map-like access... or access a
   /// corresponding vector of names
   AUXSTORE_OBJECT_SETTER_AND_GETTER( TruthEvent_v1, std::vector< float >,
                                      weights, setWeights )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthEvent_v1, float, crossSection,
                                         setCrossSection )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthEvent_v1, float,
                                         crossSectionError,
                                         setCrossSectionError )

   void TruthEvent_v1::setCrossSection( float value, float error ) {

      setCrossSection( value );
      setCrossSectionError( error );
      return;
   }

   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   // Optional PDF info accessors

   bool TruthEvent_v1::pdfInfoParameter( int& value,
                                         PdfParam information ) const {

      // Look for the accessor object:
      auto* acc = pdfInfoAccessorV1Int( information );
      if( ! acc ) return false;

      // Get the value:
      value = ( *acc )( *this );
      return true;
   }

   bool TruthEvent_v1::pdfInfoParameter( float& value,
                                         PdfParam information ) const {

      // Look for the accessor object:
      auto* acc = pdfInfoAccessorV1Float( information );
      if( ! acc ) return false;

      // Get the value:
      value = ( *acc )( *this );
      return true;
   }

   bool TruthEvent_v1::setPdfInfoParameter( int value,
                                            PdfParam information ) {

      // Look for the accessor object:
      auto* acc = pdfInfoAccessorV1Int( information );
      if( ! acc ) return false;

      // Set the value:
      ( *acc )( *this ) = value;
      return true;
   }

   bool TruthEvent_v1::setPdfInfoParameter( float value,
                                            PdfParam information ) {

      // Look for the accessor object:
      auto* acc = pdfInfoAccessorV1Float( information );
      if( ! acc ) return false;

      // Set the value:
      ( *acc )( *this ) = value;
      return true;
   }

   TruthEvent_v1::PdfInfo::PdfInfo()
      : pdgId1( 0 ), pdgId2( 0 ), pdfId1( -1 ), pdfId2( -1 ),
        x1( NAN ), x2( NAN ), Q( NAN ), xf1( NAN ), xf2( NAN ) {

   }

   bool TruthEvent_v1::PdfInfo::valid() const {

      return ( ( pdgId1 != 0 ) && ( pdgId2 != 0 ) &&
               ( pdfId1 >= 0 ) && ( pdfId2 >= 0 ) &&
               ( ! std::isnan( x1 ) ) && ( ! std::isnan( x2 ) ) &&
               ( ! std::isnan( Q ) ) &&
               ( ! std::isnan( xf1 ) ) && ( ! std::isnan( xf2 ) ) );
   }

   TruthEvent_v1::PdfInfo TruthEvent_v1::pdfInfo() const {

      // The result object:
      PdfInfo rtn;

      // Retrieve all of its elements:
      pdfInfoParameter( rtn.pdgId1, PDGID1 );
      pdfInfoParameter( rtn.pdgId2, PDGID2 );
      pdfInfoParameter( rtn.pdfId1, PDFID1 );
      pdfInfoParameter( rtn.pdfId2, PDFID2 );
      pdfInfoParameter( rtn.x1,     X1 );
      pdfInfoParameter( rtn.x2,     X2 );
      pdfInfoParameter( rtn.Q,      Q );
      pdfInfoParameter( rtn.xf1,    XF1 );
      pdfInfoParameter( rtn.xf2,    XF2 );

      return rtn;
   }

   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   // Optional heavy ion accessors

   bool TruthEvent_v1::heavyIonParameter( int& value,
                                          HIParam information ) const {

      // Look for the accessor object:
      auto* acc = heavyIonAccessorV1Int( information );
      if( ! acc ) return false;

      // Get the value:
      value = ( *acc )( *this );
      return true;
   }

   bool TruthEvent_v1::heavyIonParameter( float& value,
                                          HIParam information ) const {

      // Look for the accessor object:
      auto* acc = heavyIonAccessorV1Float( information );
      if( ! acc ) return false;

      // Get the value:
      value = ( *acc )( *this );
      return true;
   }

   bool TruthEvent_v1::setHeavyIonParameter( int value,
                                             HIParam information ) {

      // Look for the accessor object:
      auto* acc = heavyIonAccessorV1Int( information );
      if( ! acc ) return false;

      // Set the value:
      ( *acc )( *this ) = value;
      return true;
   }

   bool TruthEvent_v1::setHeavyIonParameter( float value,
                                             HIParam information ) {

      // Look for the accessor object:
      auto* acc = heavyIonAccessorV1Float( information );
      if( ! acc ) return false;

      // Set the value:
      ( *acc )( *this ) = value;
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   // Implementation for the links to truth particles/vertices

   // Accessor for the signal vertex
   static const SG::AuxElement::Accessor< TruthEvent_v1::TruthVertexLink_t >
      signalProcessVertexLinkAcc( "signalProcessVertexLink" );

   const TruthVertex* TruthEvent_v1::signalProcessVertex() const {

      // Check if the link variable is available:
      if( ! signalProcessVertexLinkAcc.isAvailable( *this ) ) {
         return 0;
      }

      // Get the link:
      const TruthVertexLink_t& vertLink = signalProcessVertexLinkAcc( *this );

      // Check if it's valid:
      if( ! vertLink.isValid() ) {
         return 0;
      }

      // Return the de-referenced link:
      return *vertLink;
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TruthEvent_v1,
                                      TruthEvent_v1::TruthVertexLink_t,
                                      signalProcessVertexLink,
                                      setSignalProcessVertexLink )

   // Accessors for the beam particles
   static const SG::AuxElement::Accessor< TruthEvent_v1::TruthParticleLink_t >
      beamParticle1LinkAcc( "beamParticle1Link" );
   static const SG::AuxElement::Accessor< TruthEvent_v1::TruthParticleLink_t >
      beamParticle2LinkAcc( "beamParticle2Link" );

   std::pair< const TruthParticle*, const TruthParticle* >
   TruthEvent_v1::beamParticles() const {

      // Get the pointer to the first beam particle:
      const TruthParticle* p1 = 0;
      if( beamParticle1LinkAcc.isAvailable( *this ) ) {
         const TruthParticleLink_t link = beamParticle1LinkAcc( *this );
         if( link.isValid() ) {
            p1 = *link;
         }
      }

      // Get the pointer to the second beam particle:
      const TruthParticle* p2 = 0;
      if( beamParticle2LinkAcc.isAvailable( *this ) ) {
         const TruthParticleLink_t link = beamParticle2LinkAcc( *this );
         if( link.isValid() ) {
            p2 = *link;
         }
      }

      // Construct the returned object:
      return std::make_pair( p1, p2 );
   }

   void TruthEvent_v1::setBeamParticleLinks( const TruthParticleLink_t& pcl1,
                                             const TruthParticleLink_t& pcl2 ) {

      setBeamParticle1Link( pcl1 );
      setBeamParticle2Link( pcl2 );
      return;
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TruthEvent_v1,
                                      TruthEvent_v1::TruthParticleLink_t,
                                      beamParticle1Link,
                                      setBeamParticle1Link )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( TruthEvent_v1,
                                      TruthEvent_v1::TruthParticleLink_t,
                                      beamParticle2Link,
                                      setBeamParticle2Link )

   /////////////////////////////////////////////////////////////////////////////

   Type::ObjectType TruthEvent_v1::type() const {

      return Type::TruthEvent;
   }

   void TruthEvent_v1::toPersistent() {

      if( beamParticle1LinkAcc.isAvailableWritable( *this ) ) {
         beamParticle1LinkAcc( *this ).toPersistent();
      }
      if( beamParticle2LinkAcc.isAvailableWritable( *this ) ) {
         beamParticle2LinkAcc( *this ).toPersistent();
      }
      if( signalProcessVertexLinkAcc.isAvailableWritable( *this ) ) {
         signalProcessVertexLinkAcc( *this ).toPersistent();
      }

      TruthEventBase_v1::toPersistent();
      return;
   }

} // namespace xAOD
