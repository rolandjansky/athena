/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventWeightFillerTool.cxx 603418 2014-06-24 15:37:50Z ssnyder $

// Local include(s):
#include "EventWeightFillerTool.h"

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

using xAOD::EventInfo;

namespace D3PD {

   EventWeightFillerTool::EventWeightFillerTool( const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent )
      : BlockFillerTool< EventInfo >( type, name, parent )
   {
     EventWeightFillerTool::book().ignore(); // Avoid coverity warnings.
   }


   StatusCode EventWeightFillerTool::book()
   {
     CHECK( addVariable( "mc_event_weight", m_mc_event_weight,
                         "MC weight, coming from the EventType object" ) );
     return StatusCode::SUCCESS;
   }


   StatusCode EventWeightFillerTool::fill( const EventInfo& ei )
   {
     *m_mc_event_weight   = ei.mcEventWeight();
     return StatusCode::SUCCESS;
   }

} // namespace D3PD
