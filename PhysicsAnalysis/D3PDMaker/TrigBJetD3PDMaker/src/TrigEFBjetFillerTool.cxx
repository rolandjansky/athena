/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigEFBjetFillerTool.h"

namespace D3PD {

   TrigEFBjetFillerTool::TrigEFBjetFillerTool( const std::string &type,
                                               const std::string &name,
                                               const IInterface *parent )
      : BlockFillerTool< TrigEFBjet >( type, name, parent )
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

   StatusCode TrigEFBjetFillerTool::book() {

      if( mWriteBasics ) {
         CHECK( addVariable( "roiId",  m_roiId ) );
         CHECK( addVariable( "valid",  m_valid ) );
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
      if( mWriteDetails ) {

      }

      return StatusCode::SUCCESS;
   }

   StatusCode TrigEFBjetFillerTool::fill( const TrigEFBjet &x ) {

      if( mWriteBasics ) {
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
      if( mWriteDetails ) {

      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
