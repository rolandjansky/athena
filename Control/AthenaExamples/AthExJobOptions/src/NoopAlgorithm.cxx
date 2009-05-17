/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthExJobOptions/NoopAlgorithm.h"


/////////////////////////////////////////////////////////////////////////////

NoopAlgorithm::NoopAlgorithm( const std::string& name, ISvcLocator* svcloc ) :
  AthAlgorithm( name, svcloc )
{
   /* empty */
}

//___________________________________________________________________________
StatusCode NoopAlgorithm::initialize()
{
   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode NoopAlgorithm::reinitialize()
{
   return initialize();
}

//___________________________________________________________________________
StatusCode NoopAlgorithm::execute()
{
   ATH_MSG_INFO ("performing no action");
   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode NoopAlgorithm::finalize()
{
   return StatusCode::SUCCESS;
}
