/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonFeatureObjectDecisionFillerTool.cxx 307534 2010-06-28 16:16:41Z krasznaa $


// Local include(s):
#include "MuonFeatureObjectDecisionFillerTool.h"

namespace D3PD {

   MuonFeatureObjectDecisionFillerTool::MuonFeatureObjectDecisionFillerTool(
      const std::string& type,
      const std::string& name,
      const IInterface* parent )
      : TriggerObjectDecisionFillerTool< MuonFeature >( type, name, parent ) {
   }
} // namespace D3PD
