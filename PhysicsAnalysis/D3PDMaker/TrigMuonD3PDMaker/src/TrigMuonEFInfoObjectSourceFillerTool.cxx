/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFInfoObjectSourceFillerTool.cxx 514304 2012-08-20 08:37:38Z krasznaa $

// EDM include(s):
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"

// Local include(s):
#include "TrigMuonEFInfoObjectSourceFillerTool.h"

namespace D3PD {

   TrigMuonEFInfoObjectSourceFillerTool::
   TrigMuonEFInfoObjectSourceFillerTool( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
      : TriggerObjectSourceFillerTool< TrigMuonEFInfo >( type, name, parent ) {

   }

} // namespace D3PD
