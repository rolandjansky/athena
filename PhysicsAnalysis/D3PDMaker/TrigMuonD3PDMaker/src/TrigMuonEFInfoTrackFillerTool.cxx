/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFInfoTrackFillerTool.cxx 502736 2012-05-27 22:05:57Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigMuonEFInfoTrackFillerTool.h"

namespace D3PD {

   TrigMuonEFInfoTrackFillerTool::TrigMuonEFInfoTrackFillerTool( const std::string& type,
                                                                 const std::string& name,
                                                                 const IInterface* parent )
      : BlockFillerTool< TrigMuonEFInfoTrack >( type, name, parent )
   {
     book().ignore(); // Avoid coverity warnings.
   }

   StatusCode TrigMuonEFInfoTrackFillerTool::book() {

      CHECK( addVariable( "MuonType", m_muonType ) );

      return StatusCode::SUCCESS;
   }

   StatusCode TrigMuonEFInfoTrackFillerTool::fill( const TrigMuonEFInfoTrack& muon ) {

      *m_muonType = muon.MuonType();

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
