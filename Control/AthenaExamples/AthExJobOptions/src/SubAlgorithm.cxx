/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SubAlgorithm.h"


/////////////////////////////////////////////////////////////////////////////

SubAlgorithm::SubAlgorithm(const std::string& name, ISvcLocator* svcloc ) :
  AthAlgorithm( name, svcloc )
{
  
// declare any properties here

}

//___________________________________________________________________________
StatusCode SubAlgorithm::initialize()
{

// perform necessary one-of initialization

   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode SubAlgorithm::reinitialize()
{

// perform necessary re-initialization

   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode SubAlgorithm::start()
{

// perform work needed before event processing starts

   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode SubAlgorithm::execute()
{

// perform work done every event

   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode SubAlgorithm::stop()
{

// perform work done after event processing stopped

   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode SubAlgorithm::finalize()
{

// perform work done at shutdown

   return StatusCode::SUCCESS;
}
