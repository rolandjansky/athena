/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TileMuFeatureObjectDecisionFillerTool.cxx 307534 2010-06-28 16:16:41Z krasznaa $


// Local include(s):
#include "TileMuFeatureObjectDecisionFillerTool.h"

namespace D3PD {

   TileMuFeatureObjectDecisionFillerTool::TileMuFeatureObjectDecisionFillerTool(
      const std::string& type,
      const std::string& name,
      const IInterface* parent )
      : TriggerObjectDecisionFillerTool< TileMuFeature >( type, name, parent ) {
   }
} // namespace D3PD
