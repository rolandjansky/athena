/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventDisplayFilters/FilterUsingMBTSTiming.h"

#include "TileIdentifier/TileTBID.h"
#include "TileEvent/TileContainer.h"

/****
 * Simple filter for collisions based on the difference of the average MBTS hit time
 * @author: Sebastian Boeser, Emily Nurse
 ****/

FilterUsingMBTSTiming::FilterUsingMBTSTiming(const std::string& name, ISvcLocator* pSvcLocator)  :  AthAlgorithm(name, pSvcLocator)
{
  declareProperty("StoreGateKey", m_mbtsContainerName = "MBTSContainer", "StoreGate key of the MBTS container (default=\"MBTSContainer\")");
	declareProperty("CellChargeThreshold", m_MBTSThreshold= 40./222. ,"Only count cell above this threshold (default = 40/222[pC])");
	declareProperty("MaxTimeDifference", m_maxTimeDifference=10., "Maximum difference of the average time of hits on A and C-side (default=10[ns])");
}

StatusCode FilterUsingMBTSTiming::initialize()
{

  //Get tile identifier helper
  if (detStore()->retrieve(m_tileTBID).isFailure()){
    msg(MSG::ERROR) << "Unable to retrieve TileTBID helper" << endreq;
    return StatusCode::FAILURE;
  }
    
	
	return StatusCode::SUCCESS;
}

StatusCode FilterUsingMBTSTiming::execute()
{

  //Retrieve container from StoreGate
  const TileCellContainer *tileCellCnt = NULL;
  if (evtStore()->retrieve(tileCellCnt, m_mbtsContainerName).isFailure()) {
    msg(MSG::WARNING) << "Unable to retrieving MBTS container with name " << m_mbtsContainerName << endreq;
    return StatusCode::SUCCESS;
  }

  //Calculate average time for A and C side
  float timeA = 0.;
  float timeC = 0.;
  unsigned int countA = 0;
  unsigned int countC = 0;

  //Loop over tile cells
  TileCellContainer::const_iterator MBTSCellItr = tileCellCnt->begin();
  TileCellContainer::const_iterator MBTSCellsEnd = tileCellCnt->end();
  for (; MBTSCellItr != MBTSCellsEnd; ++MBTSCellItr) {

    // Discriminate the signals
	  if ((*MBTSCellItr)->energy() < m_MBTSThreshold) continue ;

	  msg(MSG::DEBUG) << "Energy = " << (*MBTSCellItr)->energy() << " pC\t";
	  msg(MSG::DEBUG) << "Time = " << (*MBTSCellItr)->time() << " ns\t";
    msg(MSG::DEBUG) << "Side = " << (( m_tileTBID->type((*MBTSCellItr)->ID()) > 0 ) ? "A" : "C") << endreq;

	  // cache type, module and channel
	  // MBTS Id type is  "side"  +/- 1
	  if (m_tileTBID->type((*MBTSCellItr)->ID()) > 0)  {
	    timeA +=  (*MBTSCellItr)->time();
	    countA++ ; 
	  } else {	
	    timeC +=  (*MBTSCellItr)->time();
	    countC++;
	  }
  } 
		
  //Make sure we have a least n hits on each side
  if ( countA < 1 || countC < 1 ){
    msg(MSG::INFO) << "Need at least one hit on each side" << endreq;
    setFilterPassed(false);
    msg(MSG::INFO ) << "Event is rejected" << endreq;
    return StatusCode::SUCCESS;
  }

  // Calculate the time difference
  float timeDiff = fabs(timeA/countA - timeC/countC);
  msg(MSG::INFO) << "Calculated time difference of " << timeDiff << " ns" << endreq;

  //And cut
  if (timeDiff <= m_maxTimeDifference) {
    setFilterPassed(true);
    msg(MSG::INFO ) << "Event is accepted" << endreq;
  } else {
    setFilterPassed(false);
    msg(MSG::INFO ) << "Event is rejected" << endreq;
  }

	return StatusCode::SUCCESS;
}

StatusCode FilterUsingMBTSTiming::finalize()
{
	return StatusCode::SUCCESS;
}
