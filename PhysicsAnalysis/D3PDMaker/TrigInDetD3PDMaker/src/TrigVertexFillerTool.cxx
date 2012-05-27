/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigVertexFillerTool.cxx 502719 2012-05-27 21:28:59Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigInDetEvent/TrigVertex.h"

// Local include(s):
#include "TrigVertexFillerTool.h"

namespace D3PD {

   /// Size of the covariance array in TrigVertex
   static const int COVARIANCE_SIZE = 6;

   TrigVertexFillerTool::TrigVertexFillerTool( const std::string& type,
                                                       const std::string& name,
                                                       const IInterface* parent )
      : BlockFillerTool< TrigVertex >( type, name, parent )
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

   /**
    * Here the output variables are declared to the D3PDMaker framework.
    */
   StatusCode TrigVertexFillerTool::book() {

      if( m_writeBasicInfo ) {
         CHECK( addVariable( "algorithmId", m_algId,
                             "Vertex author (NULLID=0, SITRACKID=1, IDSCANID=2, "
                             "HISTOPRMVTXSITRACKID=3, VKALSECVTXID=4, BPHYSSECID=5, "
                             "BSFULLSITRACKID=6, BSFULLIDSCANID=7, BSSPLITSITRACKID=8, "
                             "BSSPLITIDSCANID=9, HISTOPRMVTXIDSCANID=10, HISTOPRMVTXEFID=11)" ) );
         CHECK( addVariable( "chi2", m_chi2,
                             "Chi^2 of the vertex fit" ) );
         CHECK( addVariable( "nDoF", m_nDoF,
                             "Number of degree-of-freedom of the vertex fit" ) ); 
         CHECK( addVariable( "x",    m_x,
                             "X-position" ) ); 
         CHECK( addVariable( "y",    m_y,
                             "Y-position" ) ); 
         CHECK( addVariable( "z",    m_z,
                             "Z-position" ) );
      }
      if( m_writeHypoInfo ) {
         CHECK( addVariable( "mass", m_mass,
                             "Vertex mass estimated after the vertex fit" ) );
         CHECK( addVariable( "massVariance", m_massVariance,
                             "Variance of the vertex mass estimate" ) ); 
         CHECK( addVariable( "energyFraction",  m_energyFraction,
                             "Energy ratio E(secondary vertex)/E(jet)" ) );
         CHECK( addVariable( "nTwoTrackSecVtx", m_nTwoTracksSecVtx,
                             "Number of 2-track vertices" ) );
      }
      if( m_writeDetailedInfo ) {
         CHECK( addVariable( "covariance", m_covariance,
                             "Covariance of the vertex position, packed" ) );
      }

      return StatusCode::SUCCESS;
   }

   /**
    * Here the output variables are filled with the information coming from a
    * TrigVertex object.
    */
   StatusCode TrigVertexFillerTool::fill( const TrigVertex& v ) {

      if( m_writeBasicInfo ) {
         *m_algId = v.algorithmId();
         *m_chi2 = v.chi2();
         *m_nDoF = v.ndof();
         *m_x    = v.x();
         *m_y    = v.y();
         *m_z    = v.z();
      }
      if( m_writeHypoInfo ) {
         *m_mass = v.mass();
         *m_massVariance = v.massVariance();
         *m_energyFraction = v.energyFraction(); 
         *m_nTwoTracksSecVtx = v.nTwoTracksSecVtx();
      }
      if( m_writeDetailedInfo ) {
         TrigVertex& vert = const_cast< TrigVertex& >( v );
         m_covariance->clear();
         for( int i = 0; i < COVARIANCE_SIZE; ++i ) {
            m_covariance->push_back( static_cast< float >( vert.cov()[ i ] ) );
         }
      }

      return StatusCode::SUCCESS;
   }

}
