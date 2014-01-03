/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFInfoTrackAssociationTool.cxx 502736 2012-05-27 22:05:57Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigMuonEFInfoTrackAssociationTool.h"

namespace D3PD {

   TrigMuonEFInfoTrackAssociationTool::TrigMuonEFInfoTrackAssociationTool( const std::string& type,
                                                                           const std::string& name,
                                                                           const IInterface* parent )
     : MultiAssociationTool< TrigMuonEFInfo, TrigMuonEFInfoTrack >( type, name, parent ),
       m_tracks(0)
   {
   }

   StatusCode TrigMuonEFInfoTrackAssociationTool::reset( const TrigMuonEFInfo& ef_info ) {

      m_tracks = ef_info.TrackContainer();
      m_currentTrack = m_tracks->begin();

      return StatusCode::SUCCESS;
   }

   const TrigMuonEFInfoTrack* TrigMuonEFInfoTrackAssociationTool::next() {

      if( ! m_tracks ) {
         REPORT_MESSAGE( MSG::ERROR ) << "next() called before the tool was set up using reset(...)!";
         return 0;
      }

      if( m_currentTrack != m_tracks->end() ) {
         const TrigMuonEFInfoTrack* result = *m_currentTrack;
         ++m_currentTrack;
         return result;
      } else {
         return 0;
      }
   }

} // namespace D3PD
