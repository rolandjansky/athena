/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigL2BjetFillerTool.h"


namespace D3PD {

   TrigL2BjetFillerTool::TrigL2BjetFillerTool( const std::string &type,
                                               const std::string &name,
                                               const IInterface *parent )
      : BlockFillerTool< TrigL2Bjet >( type, name, parent ),
        m_iBeamCondSvc( "BeamCondSvc", name ),
        m_beamCondAvailable(0)
   {
      // Avoid coverity warnings.
      mWriteBasics = true;
      mWriteHypoVars = true;
      mWriteDetails = true;
      book().ignore();

      declareProperty( "WriteBasics", mWriteBasics = false,
                       "Write basic information" );
      declareProperty( "WriteHypoVars", mWriteHypoVars = false,
                       "Write variables used at hypo" );
      declareProperty( "WriteDetails", mWriteDetails = false,
                       "Write detailed variables" );
   }

   StatusCode TrigL2BjetFillerTool::initialize() {

      if( m_iBeamCondSvc.retrieve().isFailure() ) {
         m_beamCondAvailable = false;
         ATH_MSG_INFO( "BeamCondSvc *not* available, not filling beam spot variables" );
      } else {
         m_beamCondAvailable = true;
         ATH_MSG_INFO( "BeamCondSvc available, filling beam spot variables" );
      }

      return StatusCode::SUCCESS;
   }

   StatusCode TrigL2BjetFillerTool::book() {

      if( mWriteBasics ) {
         CHECK( addVariable( "roiId", m_roiId ) );
         CHECK( addVariable( "valid", m_valid ) );
         CHECK( addVariable( "prmVtx", m_prmVtx ) );
      }
      if( mWriteHypoVars ) {
         CHECK( addVariable( "xComb", m_xcomb ) );
         CHECK( addVariable( "xIP1D", m_xIP1d ) );
         CHECK( addVariable( "xIP2D", m_xIP2d ) );
         CHECK( addVariable( "xIP3D", m_xIP3d ) );
         CHECK( addVariable( "xCHI2", m_xChi2 ) );
         CHECK( addVariable( "xSV",   m_xSv ) );
         CHECK( addVariable( "xMVtx", m_xmvtx ) );
         CHECK( addVariable( "xEVtx", m_xevtx ) );
         CHECK( addVariable( "xNVtx", m_xnvtx ) );
      }
      if( mWriteDetails && m_beamCondAvailable ) {
         CHECK( addVariable( "BSx", m_BSx ) );
         CHECK( addVariable( "BSy", m_BSy ) );
         CHECK( addVariable( "BSz", m_BSz ) );

         CHECK( addVariable( "sBSx", m_sBSx ) );
         CHECK( addVariable( "sBSy", m_sBSy ) );
         CHECK( addVariable( "sBSz", m_sBSz ) );
         CHECK( addVariable( "sBSxy", m_sBSxy ) );

         CHECK( addVariable( "BTiltXZ", m_BTiltXZ ) );
         CHECK( addVariable( "BTiltYZ", m_BTiltYZ ) );

         CHECK( addVariable( "BSstatus", m_BSstatus ) );
      }

      return StatusCode::SUCCESS;
   }

   StatusCode TrigL2BjetFillerTool::fill( const TrigL2Bjet &x ) {

      if ( mWriteBasics ) {
         *m_roiId = x.roiId();
         *m_valid = x.isValid();
         *m_prmVtx = x.prmVtx();
      }
      if( mWriteHypoVars ) {
         *m_xcomb = x.xComb();
         *m_xIP1d = x.xIP1D();
         *m_xIP2d = x.xIP2D();
         *m_xIP3d = x.xIP3D();
         *m_xChi2 = x.xCHI2();
         *m_xSv = x.xSV();
         *m_xmvtx = x.xMVtx();
         *m_xevtx = x.xEVtx();
         *m_xnvtx = x.xNVtx();
      }
      if( mWriteDetails && m_beamCondAvailable ) {

         //Beam spot
         Amg::Vector3D beamSpot = m_iBeamCondSvc->beamPos();

         *m_BSx = beamSpot[0];
         *m_BSy = beamSpot[1];
         *m_BSz = beamSpot[2];

         *m_sBSx = m_iBeamCondSvc->beamSigma( 0 );
         *m_sBSy = m_iBeamCondSvc->beamSigma( 1 );
         *m_sBSz = m_iBeamCondSvc->beamSigma( 2 );

         *m_sBSxy = m_iBeamCondSvc->beamSigmaXY();

         *m_BTiltXZ = m_iBeamCondSvc->beamTilt( 0 );
         *m_BTiltYZ = m_iBeamCondSvc->beamTilt( 1 );

         *m_BSstatus = m_iBeamCondSvc->beamStatus();
      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
