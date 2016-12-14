/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
   }

   StatusCode TrackParticleCompressorTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
      ATH_MSG_INFO( "  OffDiagCovMatrixBits = " << m_offDiagCovMatrixBits );
      ATH_MSG_INFO( "  DiagCovMatrixBits    = " << m_diagCovMatrixBits );

      // Create the helper objects:
      m_offDiagCovMatrixCompressor.reset(
               new xAOD::FloatCompressor( m_offDiagCovMatrixBits ) );
      m_diagCovMatrixCompressor.reset(
               new xAOD::FloatCompressor( m_diagCovMatrixBits ) );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode
   TrackParticleCompressorTool::compress( xAOD::TrackParticle& tp ) const {

      // Get the uncompressed covariance matrix:
      xAOD::ParametersCovMatrix_t covMatrix = tp.definingParametersCovMatrix();
      for( xAOD::ParametersCovMatrix_t::Index i = 0; i < covMatrix.rows();
           ++i ) {
         for( xAOD::ParametersCovMatrix_t::Index j = 0; j < covMatrix.cols();
              ++j ) {
            // Decide which compressor to use:
            const xAOD::FloatCompressor& fc =
                  ( ( i == j ) ? *m_diagCovMatrixCompressor :
                                 *m_offDiagCovMatrixCompressor );
            // Compress the matrix element:
            covMatrix( i, j ) = fc.reduceFloatPrecision( covMatrix( i, j ) );
         }
      }
      // Clear the current values, as the next call will only add to the
      // existing matrix elements:
      tp.setDefiningParametersCovMatrixVec( std::vector< float >() );
      // Overwrite the matrix with the compressed value:
      tp.setDefiningParametersCovMatrix( covMatrix );

      // Retrun gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
