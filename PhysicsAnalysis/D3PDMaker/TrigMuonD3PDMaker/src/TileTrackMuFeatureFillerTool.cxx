/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TileTrackMuFeatureFillerTool.cxx 502736 2012-05-27 22:05:57Z ssnyder $

// Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TileTrackMuFeatureFillerTool.h"

namespace D3PD {

   TileTrackMuFeatureFillerTool::TileTrackMuFeatureFillerTool( const std::string& type,
                                                               const std::string& name,
                                                               const IInterface* parent )
      : BlockFillerTool< TileTrackMuFeature >( type, name, parent )
   {
     book().ignore(); // Avoid coverity warnings.
   }

   StatusCode TileTrackMuFeatureFillerTool::book() {

      CHECK( addVariable( "PtTR_Trk" , m_PtTR_Trk  ) );
      CHECK( addVariable( "EtaTR_Trk", m_EtaTR_Trk ) );
      CHECK( addVariable( "PhiTR_Trk", m_PhiTR_Trk ) );
      CHECK( addVariable( "Typ_IDTrk", m_Typ_IDTrk ) );

      return StatusCode::SUCCESS;
   }

   StatusCode TileTrackMuFeatureFillerTool::fill( const TileTrackMuFeature& muon ) {

      *m_PtTR_Trk  = muon.PtTR_Trk();
      *m_EtaTR_Trk = muon.EtaTR_Trk();
      *m_PhiTR_Trk = muon.PhiTR_Trk();
      *m_Typ_IDTrk = muon.Typ_IDTrk();

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
