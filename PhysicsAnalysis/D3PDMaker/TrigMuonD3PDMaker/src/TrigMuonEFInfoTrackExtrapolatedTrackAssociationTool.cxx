/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFInfoTrackExtrapolatedTrackAssociationTool.cxx 307534 2010-06-28 16:16:41Z krasznaa $

// Local include(s):
#include "TrigMuonEFInfoTrackExtrapolatedTrackAssociationTool.h"

namespace D3PD {

   TrigMuonEFInfoTrackExtrapolatedTrackAssociationTool::
   TrigMuonEFInfoTrackExtrapolatedTrackAssociationTool( const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent )
      : SingleAssociationTool< TrigMuonEFInfoTrack, TrigMuonEFTrack >( type, name, parent ) {

   }

   const TrigMuonEFTrack*
   TrigMuonEFInfoTrackExtrapolatedTrackAssociationTool::get( const TrigMuonEFInfoTrack& info_track ) {

      if( info_track.hasExtrapolatedTrack() ) {
         return info_track.ExtrapolatedTrack();
      } else {
         return 0;
      }
   }

} // namespace D3PD
