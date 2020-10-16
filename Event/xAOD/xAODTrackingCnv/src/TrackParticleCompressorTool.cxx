/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackParticleCompressorTool.cxx 789658 2016-12-14 14:28:40Z krasznaa $

// Local include(s):
#include "TrackParticleCompressorTool.h"

namespace xAODMaker {

   TrackParticleCompressorTool::
   TrackParticleCompressorTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface implemented by the tool
      declareInterface< xAODMaker::ITrackParticleCompressorTool >( this );

      // Declare the properties of the tool:
      declareProperty( "OffDiagCovMatrixBits", m_offDiagCovMatrixBits = 23,
                       "Bits kept for the off-diagonal covariance matrix "
                       "elements" );
      declareProperty( "DiagonalCovMatrixBits", m_diagCovMatrixBits = 23,
                       "Bits kept for the diagonal covariance matrix "
                       "elements" );
      declareProperty( "UseOffDiagCompr", m_useOffDiagCompr = false,
                       "Flag to control the off-diagonal compression "
                       "scheme" );
   }

   StatusCode TrackParticleCompressorTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
      ATH_MSG_INFO( "  OffDiagCovMatrixBits = " << m_offDiagCovMatrixBits );
      ATH_MSG_INFO( "  DiagCovMatrixBits    = " << m_diagCovMatrixBits );
      ATH_MSG_INFO( "  UseOffDiagCompr = " << m_useOffDiagCompr );


      // Create the helper objects:
      m_diagCovMatrixCompressor.reset(
               new CxxUtils::FloatCompressor( m_diagCovMatrixBits ) );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode
   TrackParticleCompressorTool::compress( xAOD::TrackParticle& tp ) const {

     xAOD::ParametersCovMatrix_t covMatrix = tp.definingParametersCovMatrix();
     tp.setDefiningParametersCovMatrix(covMatrix); //Separate diag and offdiag cov matrix vectors may not have been set before
     if(m_useOffDiagCompr) tp.compressDefiningParametersCovMatrixOffDiag();

     //Check if determinant got negative after compression
     //If so, undo
     xAOD::ParametersCovMatrix_t covMatrixCompr = tp.definingParametersCovMatrix();
     if(covMatrixCompr.determinant()<=0.) tp.setDefiningParametersCovMatrix(covMatrix);

     const std::vector< float > diagVec = tp.definingParametersCovMatrixDiagVec();
     std::vector< float > diagVecCompr;

     for( unsigned int i = 0; i < diagVec.size(); ++i ) {
       diagVecCompr.push_back(m_diagCovMatrixCompressor->reduceFloatPrecision( diagVec[i] ));
     }

     tp.setDefiningParametersCovMatrixDiagVec( diagVecCompr );

     auto compressOffDiag = []( const std::vector< float >& offDiagVec, int bits ) -> std::vector< float > {
       CxxUtils::FloatCompressor fc( bits );
       std::vector< float > offDiagVecCompr;
       offDiagVecCompr.reserve( offDiagVec.size() );
       for( float element : offDiagVec ) {
         offDiagVecCompr.push_back( fc.reduceFloatPrecision( element ) );
       }
       return offDiagVecCompr;
     };

     const std::vector< float > offDiagVec = tp.definingParametersCovMatrixOffDiagVec();
     tp.setDefiningParametersCovMatrixOffDiagVec( compressOffDiag( offDiagVec, m_offDiagCovMatrixBits ) );
     int offDiagComprBits = m_offDiagCovMatrixBits;

     // Need to check determinant of the matrix + its inverse to prevent any precision issue in inverse computation
     while( ( tp.definingParametersCovMatrix().determinant() <= 0.0 || tp.definingParametersCovMatrix().inverse().determinant() <= 0.0 ) &&
	    ( ++offDiagComprBits <= m_diagCovMatrixBits ) ) {
       tp.setDefiningParametersCovMatrixOffDiagVec( compressOffDiag( offDiagVec, offDiagComprBits ) );
     }

     // Return gracefully:
     return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
