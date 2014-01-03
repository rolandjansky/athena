/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CombinedMuonFeatureIDTrackAssociationTool.cxx 282713 2010-03-04 13:21:48Z krasznaa $

// Local include(s):
#include "CombinedMuonFeatureIDTrackAssociationTool.h"

namespace D3PD {

   CombinedMuonFeatureIDTrackAssociationTool::CombinedMuonFeatureIDTrackAssociationTool( const std::string& type,
                                                                                         const std::string& name,
                                                                                         const IInterface* parent )
      : SingleAssociationTool< CombinedMuonFeature, TrigInDetTrack >( type, name, parent ) {

   }

   const TrigInDetTrack* CombinedMuonFeatureIDTrackAssociationTool::get( const CombinedMuonFeature& muon ) {

      return muon.IDTrack();
   }

} // namespace D3PD
