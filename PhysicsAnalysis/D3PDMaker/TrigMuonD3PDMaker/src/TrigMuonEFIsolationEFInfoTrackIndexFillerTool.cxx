/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFIsolationEFInfoTrackIndexFillerTool.cxx 502736 2012-05-27 22:05:57Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"

// Local include(s):
#include "TrigMuonEFIsolationEFInfoTrackIndexFillerTool.h"

namespace D3PD {

   TrigMuonEFIsolationEFInfoTrackIndexFillerTool::TrigMuonEFIsolationEFInfoTrackIndexFillerTool( const std::string& type,
                                                                                                 const std::string& name,
                                                                                                 const IInterface* parent )
      : BlockFillerTool< TrigMuonEFIsolation >( type, name, parent )
   {
     book().ignore(); // Avoid coverity warnings.
   }

   StatusCode TrigMuonEFIsolationEFInfoTrackIndexFillerTool::book() {

      CHECK( addVariable( "efinfotrack_index", m_index_infotrack,
                          "Index of the TrigMuonEFInfoTrack within the TrigMuonEFInfo object" ) );

      return StatusCode::SUCCESS;
   }

   StatusCode TrigMuonEFIsolationEFInfoTrackIndexFillerTool::fill( const TrigMuonEFIsolation& muon ) {

      *m_index_infotrack = muon.trackPosition();

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
