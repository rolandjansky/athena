/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TileMuFeatureFillerTool.cxx 502736 2012-05-27 22:05:57Z ssnyder $

// Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TileMuFeatureFillerTool.h"

namespace D3PD {

   TileMuFeatureFillerTool::TileMuFeatureFillerTool( const std::string& type,
                                                                 const std::string& name,
                                                                 const IInterface* parent )
      : BlockFillerTool< TileMuFeature >( type, name, parent )
   {
      // Avoid coverity warnings.
      m_writeAngle = true;
      m_writeDetails = true;
      book().ignore();

      declareProperty( "WriteAngle",   m_writeAngle = false );
      declareProperty( "WriteDetails", m_writeDetails= false );
   }

   StatusCode TileMuFeatureFillerTool::book() {
      if(m_writeAngle){
         CHECK( addVariable( "eta"    , m_eta ) );
         CHECK( addVariable( "phi"    , m_phi ) );
      }
      if(m_writeDetails){
         CHECK( addVariable( "enedep" , m_enedep ) );
         CHECK( addVariable( "quality", m_quality ) );
      }
      return StatusCode::SUCCESS;
   }

   StatusCode TileMuFeatureFillerTool::fill( const TileMuFeature& muon ) {
      if(m_writeAngle){
         *m_eta     = muon.eta();
         *m_phi     = muon.phi();
      }
      if(m_writeDetails){
         *m_enedep  = muon.enedep();
         *m_quality = muon.quality();
      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
