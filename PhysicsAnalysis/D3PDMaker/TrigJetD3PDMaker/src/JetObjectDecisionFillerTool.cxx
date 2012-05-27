/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetObjectDecisionFillerTool.cxx 307763 2010-06-29 14:25:58Z krasznaa $

// Local include(s):
#include "JetObjectDecisionFillerTool.h"

namespace D3PD {

   JetObjectDecisionFillerTool::JetObjectDecisionFillerTool( const std::string& type,
                                                             const std::string& name,
                                                             const IInterface* parent )
      : TriggerObjectDecisionFillerTool< Jet >( type, name, parent ) {

   }

} // namespace D3PD
