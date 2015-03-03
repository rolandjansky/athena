/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CombinedMuonFeatureObjectDecisionFillerTool.cxx 307534 2010-06-28 16:16:41Z krasznaa $


// Local include(s):
#include "CombinedMuonFeatureObjectDecisionFillerTool.h"

namespace D3PD {

   CombinedMuonFeatureObjectDecisionFillerTool::CombinedMuonFeatureObjectDecisionFillerTool(
      const std::string& type,
      const std::string& name,
      const IInterface* parent )
      : TriggerObjectDecisionFillerTool< CombinedMuonFeature >( type, name, parent ) {
   }
} // namespace D3PD
