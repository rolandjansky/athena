/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <iostream>

// Local include(s):
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

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

/// Function filling one TrackParticle with information
void fill( xAOD::TrackParticle& tp ) {

   tp.setDefiningParameters( 1.0, 2.0, 1.23, 0.5, 0.25 );

   static const float covMatrix[ 15 ] = {
      1.0, 1.0, 1.0, 1.0, 1.0,
      2.0, 2.0, 2.0, 2.0, 2.0,
      3.0, 3.0, 3.0, 3.0, 3.0
   };
   static const std::vector< float >
      covMatrixVec( covMatrix, covMatrix + 15 );
   tp.setDefiningParametersCovMatrixVec( covMatrixVec );

   tp.setParametersOrigin( 0.0, 1.0, 2.0 );

   static const float parameters[ 2 ][ 6 ] = {
      { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0 },
      { 6.0, 7.0, 8.0, 9.0, 10.0, 11.0 }
   };
   std::vector< std::vector< float > > parametersVec;
   if( ! parametersVec.size() ) {
      for( int i = 0; i < 2; ++i ) {
         std::vector< float > temp( parameters[ i ],
                                    parameters[ i ] + 6 );
         parametersVec.push_back( temp );
      }
   }
   tp.setTrackParameters( parametersVec );

   tp.setParameterPosition( 0, xAOD::FirstMeasurement );
   tp.setParameterPosition( 1, xAOD::CalorimeterEntrance );

   return;
}

/// Function printing the properties of a TrackParticle
void print( const xAOD::TrackParticle& tp ) {

   std::cout << "d0 = " << tp.d0() << ", z0 = " << tp.z0()
             << ", phi0 = " << tp.phi0() << ", theta = " << tp.theta()
             << ", qOverP = " << tp.qOverP() << std::endl;
   std::cout << "definingParametersCovMatrixVec = "
             << tp.definingParametersCovMatrixVec() << std::endl;
   std::cout << "vx = " << tp.vx() << ", vy = " << tp.vy()
             << ", vz = " << tp.vz() << std::endl;
   std::cout << "numberOfParameters = " << tp.numberOfParameters() << std::endl;
   for( size_t i = 0; i < tp.numberOfParameters(); ++i ) {
      std::cout << "  - x = " << tp.parameterX( i )
                << ", y = " << tp.parameterY( i )
                << ", z = " << tp.parameterZ( i )
                << ", px = " << tp.parameterPX( i )
                << ", py = " << tp.parameterPY( i )
                << ", pz = " << tp.parameterPZ( i ) << std::endl;
   }
   std::cout << "parameterPosition( 0 ) = "
             << tp.parameterPosition( 0 ) << std::endl;
   std::cout << "parameterPosition( 1 ) = "
             << tp.parameterPosition( 1 ) << std::endl;

   return;
}

int main() {

   // Create the main containers to test:
   xAOD::TrackParticleAuxContainer aux;
   xAOD::TrackParticleContainer tpc;
   tpc.setStore( &aux );

   // Add one track particle to the container:
   xAOD::TrackParticle* p = new xAOD::TrackParticle();
   tpc.push_back( p );

   // Fill it with information:
   fill( *p );

   // Print the information:
   print( *p );

   // Print the contents of the auxiliary store:
   aux.dump();
   
   // Copy the track particle.
   xAOD::TrackParticle* p2 = new xAOD::TrackParticle(*p);
   
   // Print the information:
   print( *p2 );

   // Return gracefully:
   return 0;
}
