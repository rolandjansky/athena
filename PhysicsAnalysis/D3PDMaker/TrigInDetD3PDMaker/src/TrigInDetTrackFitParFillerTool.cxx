/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigInDetTrackFitParFillerTool.cxx 502719 2012-05-27 21:28:59Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigInDetTrackFitParFillerTool.h"

namespace D3PD {

   TrigInDetTrackFitParFillerTool::TrigInDetTrackFitParFillerTool( const std::string& type,
                                                                   const std::string& name,
                                                                   const IInterface* parent )
      : BlockFillerTool< TrigInDetTrackFitPar >( type, name, parent )
   {
      // Avoid coverity warnings.
      m_writeBasicInfo = true;
      m_writeHypoInfo = true;
      m_writeDetailedInfo = true;
      book().ignore();

      declareProperty( "WriteBasicInfo", m_writeBasicInfo = false,
                       "Writes basic information" );
      declareProperty( "WriteHypoInfo", m_writeHypoInfo = false,
                       "Writes information for hypothesis" );
      declareProperty( "WriteDetailedInfo", m_writeDetailedInfo = false,
                       "Writes detailed information" );
   }

   StatusCode TrigInDetTrackFitParFillerTool::book() {

      if( m_writeBasicInfo ) {

         CHECK( addVariable( "a0",   m_a0,
                             "Transverse impact parameter" ) );
         CHECK( addVariable( "z0",   m_z0,
                             "Longitudinal impact parameter" ) );
         CHECK( addVariable( "phi0", m_phi0,
                             "Azimuthal angle of the momentum" ) );
         CHECK( addVariable( "eta",  m_eta,
                             "Pseudorapidity" ) );
         CHECK( addVariable( "pt",   m_pT,
                             "Transverse momentum" ) );

      }
      if( m_writeDetailedInfo ) {

         CHECK( addVariable( "covariance",  m_covariance,
                             "Covariance (packed) of track parameters" ) );
         CHECK( addVariable( "surfaceType", m_surfaceType,
                             "Surface type (PERIGEE=0, BARREL=1, ENDCAP=2, UNDEFINED=3)" ) );
         CHECK( addVariable( "surfaceCoordinate", m_surfaceCoordinate,
                             "Surface reference coordinate (radius or z-position) for "
                             "non-perigee parameters" ) );

      }

      return StatusCode::SUCCESS;
   }

   StatusCode TrigInDetTrackFitParFillerTool::fill( const TrigInDetTrackFitPar& fp ) {

      if( m_writeBasicInfo ) {

         *m_a0   = fp.a0();
         *m_z0   = fp.z0();
         *m_phi0 = fp.phi0();
         *m_eta  = fp.eta();
         *m_pT   = fp.pT();

      }
      if( m_writeDetailedInfo ) {

         if( ! fp.cov() ) {

            REPORT_MESSAGE( MSG::DEBUG ) << "TrigInDetTrackFitPars has no covariance";
            for( int i = 0; i < 15 ; ++i ) {
               m_covariance->push_back( 0.0 );
            }

         } else {

            m_covariance->clear();
            for( size_t i = 0; i < fp.cov()->size(); ++i ) {
               m_covariance->push_back( static_cast< float >( ( *fp.cov() )[ i ] ) );
            }

         }

         switch( fp.surfaceType() ) {

         case TrigInDetTrackFitPar::PERIGEE:
            *m_surfaceType = 0;
            break;
         case TrigInDetTrackFitPar::BARREL:
            *m_surfaceType = 1;
            break;
         case TrigInDetTrackFitPar::ENDCAP:
            *m_surfaceType = 2;
            break;
         case TrigInDetTrackFitPar::UNDEFINED:
            *m_surfaceType = 3;
            break;
         default:
            REPORT_MESSAGE( MSG::WARNING )
               << "Surface type not recognised. Saving as UNDEFINED.";
            *m_surfaceType = 3;
            break;
         }

         *m_surfaceCoordinate = fp.surfaceCoordinate();

      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
