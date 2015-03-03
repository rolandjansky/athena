/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IsoMuonFeatureObjectDecisionFillerTool.cxx 307534 2010-06-28 16:16:41Z krasznaa $


// Local include(s):
#include "IsoMuonFeatureObjectDecisionFillerTool.h"

namespace D3PD {

   IsoMuonFeatureObjectDecisionFillerTool::IsoMuonFeatureObjectDecisionFillerTool(
      const std::string& type,
      const std::string& name,
      const IInterface* parent )
      : TriggerObjectDecisionFillerTool< IsoMuonFeature >( type, name, parent ) {
   }
} // namespace D3PD
