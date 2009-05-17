/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthExJobOptions/SubAlgorithm.h"


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
StatusCode SubAlgorithm::beginRun()
{

// perform work needed at start of every run

   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode SubAlgorithm::execute()
{

// perform work done every event

   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode SubAlgorithm::endRun()
{

// perform work done at end of every run

   return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode SubAlgorithm::finalize()
{

// perform work done at shutdown

   return StatusCode::SUCCESS;
}
