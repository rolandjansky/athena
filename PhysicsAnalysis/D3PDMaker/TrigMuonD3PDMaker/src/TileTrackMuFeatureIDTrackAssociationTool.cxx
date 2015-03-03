/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TileTrackMuFeatureIDTrackAssociationTool.cxx 307534 2010-06-28 16:16:41Z krasznaa $

// Local include(s):
#include "TileTrackMuFeatureIDTrackAssociationTool.h"

namespace D3PD {

   TileTrackMuFeatureIDTrackAssociationTool::TileTrackMuFeatureIDTrackAssociationTool( const std::string& type,
                                                                                       const std::string& name,
                                                                                       const IInterface* parent )
      : SingleAssociationTool< TileTrackMuFeature, TrigInDetTrack >( type, name, parent ) {

   }

   const TrigInDetTrack* TileTrackMuFeatureIDTrackAssociationTool::get( const TileTrackMuFeature& muon ) {
      if(muon.IDScanLink().isValid()) return muon.IDScanOutput();
      return 0;
   }

} // namespace D3PD
