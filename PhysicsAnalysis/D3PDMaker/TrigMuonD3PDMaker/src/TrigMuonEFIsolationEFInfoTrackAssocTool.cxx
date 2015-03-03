/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFIsolationEFInfoTrackAssocTool.cxx 493591 2012-04-02 12:06:17Z kanno $

// Local include(s):
#include "TrigMuonEFIsolationEFInfoTrackAssocTool.h"

namespace D3PD {

   TrigMuonEFIsolationEFInfoTrackAssocTool::TrigMuonEFIsolationEFInfoTrackAssocTool( const std::string& type,
                                                                                     const std::string& name,
                                                                                     const IInterface* parent )
      : SingleAssociationTool< TrigMuonEFIsolation, TrigMuonEFInfoTrack >( type, name, parent ) {
   }

   const TrigMuonEFInfoTrack* TrigMuonEFIsolationEFInfoTrackAssocTool::get( const TrigMuonEFIsolation& muon ) {

      return muon.getEFMuonInfoTrack();
   }

} // namespace D3PD
