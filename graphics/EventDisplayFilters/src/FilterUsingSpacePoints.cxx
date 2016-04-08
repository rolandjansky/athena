/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventDisplayFilters/FilterUsingSpacePoints.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkSpacePoint/SpacePointContainer.h"

#include <sstream>

/****
 * simple filter for splash events based on MBTS multiplicites
 * @author: Brian Thomas Martin
 ****/

	FilterUsingSpacePoints::FilterUsingSpacePoints(const std::string& name, ISvcLocator* pSvcLocator)  :  Algorithm(name, pSvcLocator)
{
	declareProperty("NumberOfSpacePoints", m_SpacePoints_required=4);
	declareProperty("SpacePointContainer", m_SpacePointContainerName="SCT_SpacePoints");
}

StatusCode FilterUsingSpacePoints::initialize()
{
	MsgStream log(msgSvc(), name());
	StatusCode sc;
	log << MSG::INFO << "Initializing " << name() << endreq;
	  sc = service( "StoreGateSvc", m_eventStore);
	  if( sc.isFailure() ) {
		  log << MSG::FATAL << name() << ": Unable to locate Service StoreGateSvc" << endreq;
		  return sc;
	  }
	
	return StatusCode::SUCCESS;
}

StatusCode FilterUsingSpacePoints::execute()
{
	MsgStream log(msgSvc(), name());

  //Get the space point container
  const SpacePointContainer* m_SpacePointContainer;

  if ( m_eventStore->retrieve(m_SpacePointContainer,  m_SpacePointContainerName).isFailure() ){
     log << MSG::INFO << "Unable to retrieve SpacePoint container with name " << m_SpacePointContainerName << endreq;
     return StatusCode::FAILURE;
  }
  

  unsigned long NSpacePoints = 0;

  //Get an iterator over the SpacePoint container itself  
  SpacePointContainer::const_iterator SpacePointCollItr = m_SpacePointContainer->begin();
  for (; SpacePointCollItr!=  m_SpacePointContainer->end(); ++SpacePointCollItr)
       NSpacePoints += (**SpacePointCollItr).size();


  //be verbose
  log << MSG::DEBUG << "Number of SpacePoints is " << NSpacePoints << endreq;

  //And cut
  if (NSpacePoints >= m_SpacePoints_required) {
    setFilterPassed(true);
    log << MSG::INFO << "Event is accepted" << endreq;
  } else {
    setFilterPassed(false);
    log << MSG::INFO << "Event is rejected" << endreq;
  }

	return StatusCode::SUCCESS;
}

StatusCode FilterUsingSpacePoints::finalize()
{
	return StatusCode::SUCCESS;
}
