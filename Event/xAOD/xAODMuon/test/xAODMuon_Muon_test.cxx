/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTracking_TrackParticle_test.cxx 588420 2014-03-19 14:15:25Z krasznaa $

// System include(s):
#include <iostream>

// Local include(s):
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"

template< typename T >
std::ostream& operator<< ( std::ostream& out,
                           const std::vector< T >& vec ) {

   out << "[";
   for( size_t i = 0; i < vec.size(); ++i ) {
      out << vec[ i ];
      if( i < vec.size() - 1 ) {
         out << ", ";
      }
   }
   out << "]";
   return out;
}

/// Function filling one Muon with information
void fill( xAOD::Muon& muon ) {

   muon.setP4( 1.0, 2.0, 3.0 );
   muon.setAuthor(xAOD::Muon::MuidCo);
   muon.setAllAuthors(0x10);
   muon.setMuonType(xAOD::Muon::Combined);
   float value=1.0;
   muon.setParameter(value, xAOD::Muon::spectrometerFieldIntegral);
   muon.setParameter(value, xAOD::Muon::momentumBalanceSignificance);
   muon.setQuality(xAOD::Muon::Medium);
   
   muon.setParameter(1, xAOD::Muon::msInnerMatchDOF);
   
   // static const float covMatrix[ 15 ] = {
   //    1.0, 1.0, 1.0, 1.0, 1.0,
   //    2.0, 2.0, 2.0, 2.0, 2.0,
   //    3.0, 3.0, 3.0, 3.0, 3.0
   // };
   // static const std::vector< float >
   //    covMatrixVec( covMatrix, covMatrix + 15 );
   // tp.setDefiningParametersCovMatrixVec( covMatrixVec );

   // tp.setParametersOrigin( 0.0, 1.0, 2.0 );

   // tatic const float parameters[ 2 ][ 6 ] = {
//       { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0 },
//       { 6.0, 7.0, 8.0, 9.0, 10.0, 11.0 }
//    };
//    static std::vector< std::vector< float > > parametersVec;
//    if( ! parametersVec.size() ) {
//       for( int i = 0; i < 2; ++i ) {
//          std::vector< float > temp( parameters[ i ],
//                                     parameters[ i ] + 6 );
//          parametersVec.push_back( temp );
//       }
//    }
//    tp.setTrackParameters( parametersVec );
//
//    tp.setParameterPosition( 0, xAOD::FirstMeasurement );
//    tp.setParameterPosition( 1, xAOD::CalorimeterEntrance );

   return;
}

/// Function printing the properties of a TrackParticle
void print( const xAOD::Muon& muon ) {

   std::cout << "pt = " << muon.pt() << ", eta = " << muon.eta()
             << ", phi = " << muon.phi() << ", m = " << muon.m()<< ", e = " << muon.e()<<std::endl;
   

   return;
}

int main() {

   // Create the main containers to test:
   xAOD::MuonAuxContainer aux;
   xAOD::MuonContainer tpc;
   tpc.setStore( &aux );

   // Add one track particle to the container:
   xAOD::Muon* p = new xAOD::Muon();
   tpc.push_back( p );

   // Fill it with information:
   fill( *p );

   // Print the information:
   print( *p );

   // // Print the contents of the auxiliary store:
   // aux.dump();

   // Return gracefully:
   return 0;
}
