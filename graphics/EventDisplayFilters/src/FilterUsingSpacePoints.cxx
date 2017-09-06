/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventDisplayFilters/FilterUsingSpacePoints.h"
#include "TrkSpacePoint/SpacePointContainer.h"

#include <sstream>

/****
 * simple filter for splash events based on MBTS multiplicites
 * @author: Brian Thomas Martin
 ****/

	FilterUsingSpacePoints::FilterUsingSpacePoints(const std::string& name, ISvcLocator* pSvcLocator)  :  AthAlgorithm(name, pSvcLocator)
{
	declareProperty("NumberOfSpacePoints", m_SpacePoints_required=4);
	declareProperty("SpacePointContainer", m_SpacePointContainerName="SCT_SpacePoints");
}

StatusCode FilterUsingSpacePoints::initialize()
{
  ATH_MSG_INFO( "Initializing " << name()  );
  return StatusCode::SUCCESS;
}

StatusCode FilterUsingSpacePoints::execute()
{
  //Get the space point container
  const SpacePointContainer* m_SpacePointContainer;

  ATH_CHECK( evtStore()->retrieve(m_SpacePointContainer,  m_SpacePointContainerName) );

  unsigned long NSpacePoints = 0;

  //Get an iterator over the SpacePoint container itself  
  SpacePointContainer::const_iterator SpacePointCollItr = m_SpacePointContainer->begin();
  for (; SpacePointCollItr!=  m_SpacePointContainer->end(); ++SpacePointCollItr)
       NSpacePoints += (**SpacePointCollItr).size();


  //be verbose
  ATH_MSG_DEBUG( "Number of SpacePoints is " << NSpacePoints  );

  //And cut
  if (NSpacePoints >= m_SpacePoints_required) {
    setFilterPassed(true);
    ATH_MSG_INFO( "Event is accepted"  );
  } else {
    setFilterPassed(false);
    ATH_MSG_INFO( "Event is rejected"  );
  }

  return StatusCode::SUCCESS;
}

StatusCode FilterUsingSpacePoints::finalize()
{
	return StatusCode::SUCCESS;
}
