/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetObjectSourceFillerTool.cxx 358948 2011-04-15 09:05:30Z krasznaa $

// Local include(s):
#include "JetObjectSourceFillerTool.h"

namespace D3PD {

   JetObjectSourceFillerTool::JetObjectSourceFillerTool( const std::string& type,
                                                         const std::string& name,
                                                         const IInterface* parent )
      : TriggerObjectSourceFillerTool< Jet >( type, name, parent ) {

   }

} // namespace D3PD
