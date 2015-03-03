/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TileTrackMuFeatureTileMuFeatureAssociationTool.cxx 307534 2010-06-28 16:16:41Z krasznaa $

// Local include(s):
#include "TileTrackMuFeatureTileMuFeatureAssociationTool.h"

namespace D3PD {

   TileTrackMuFeatureTileMuFeatureAssociationTool::TileTrackMuFeatureTileMuFeatureAssociationTool( const std::string& type,
                                                                                                   const std::string& name,
                                                                                                   const IInterface* parent )
      : SingleAssociationTool< TileTrackMuFeature, TileMuFeature >( type, name, parent ) {

   }

   const TileMuFeature* TileTrackMuFeatureTileMuFeatureAssociationTool::get( const TileTrackMuFeature& muon ) {
      if(muon.TileMuLink().isValid())return muon.TileMuOutput();
      return 0;
   }

} // namespace D3PD
