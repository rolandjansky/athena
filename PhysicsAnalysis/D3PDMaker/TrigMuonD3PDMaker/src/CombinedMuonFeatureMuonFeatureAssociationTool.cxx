/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CombinedMuonFeatureMuonFeatureAssociationTool.cxx 282713 2010-03-04 13:21:48Z krasznaa $

// Local include(s):
#include "CombinedMuonFeatureMuonFeatureAssociationTool.h"

namespace D3PD {

   CombinedMuonFeatureMuonFeatureAssociationTool::CombinedMuonFeatureMuonFeatureAssociationTool( const std::string& type,
                                                                                                 const std::string& name,
                                                                                                 const IInterface* parent )
      : SingleAssociationTool< CombinedMuonFeature, MuonFeature >( type, name, parent ) {

   }

   const MuonFeature* CombinedMuonFeatureMuonFeatureAssociationTool::get( const CombinedMuonFeature& muon ) {

      return muon.muFastTrack();
   }

} // namespace D3PD
