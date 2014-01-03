/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFInfoTrackSpectrometerTrackAssociationTool.cxx 307534 2010-06-28 16:16:41Z krasznaa $

// Local include(s):
#include "TrigMuonEFInfoTrackSpectrometerTrackAssociationTool.h"

namespace D3PD {

   TrigMuonEFInfoTrackSpectrometerTrackAssociationTool::
   TrigMuonEFInfoTrackSpectrometerTrackAssociationTool( const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent )
      : SingleAssociationTool< TrigMuonEFInfoTrack, TrigMuonEFTrack >( type, name, parent ) {

   }

   const TrigMuonEFTrack*
   TrigMuonEFInfoTrackSpectrometerTrackAssociationTool::get( const TrigMuonEFInfoTrack& info_track ) {

      if( info_track.hasSpectrometerTrack() ) {
         return info_track.SpectrometerTrack();
      } else {
         return 0;
      }
   }

} // namespace D3PD
