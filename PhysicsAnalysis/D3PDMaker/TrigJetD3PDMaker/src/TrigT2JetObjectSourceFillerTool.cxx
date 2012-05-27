/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Local include(s):
#include "TrigT2JetObjectSourceFillerTool.h"

namespace D3PD {

   TrigT2JetObjectSourceFillerTool::TrigT2JetObjectSourceFillerTool( const std::string& type,
                                                         const std::string& name,
                                                         const IInterface* parent )
      : TriggerObjectSourceFillerTool< TrigT2Jet >( type, name, parent ) {

   }

} // namespace D3PD
