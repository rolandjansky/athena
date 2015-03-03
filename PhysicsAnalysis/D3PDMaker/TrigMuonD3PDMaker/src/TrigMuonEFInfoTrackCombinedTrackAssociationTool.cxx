/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFInfoTrackCombinedTrackAssociationTool.cxx 307534 2010-06-28 16:16:41Z krasznaa $

// Local include(s):
#include "TrigMuonEFInfoTrackCombinedTrackAssociationTool.h"

namespace D3PD {

   TrigMuonEFInfoTrackCombinedTrackAssociationTool::
   TrigMuonEFInfoTrackCombinedTrackAssociationTool( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent )
      : SingleAssociationTool< TrigMuonEFInfoTrack, TrigMuonEFCbTrack >( type, name, parent ) {

   }

   const TrigMuonEFCbTrack*
   TrigMuonEFInfoTrackCombinedTrackAssociationTool::get( const TrigMuonEFInfoTrack& info_track ) {

      if( info_track.hasCombinedTrack() ) {
         return info_track.CombinedTrack();
      } else {
         return 0;
      }
   }

} // namespace D3PD
