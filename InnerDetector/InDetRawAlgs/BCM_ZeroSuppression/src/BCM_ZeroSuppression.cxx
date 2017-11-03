/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/StatusCode.h"

#include "BCM_ZeroSuppression/BCM_ZeroSuppression.h"

#include "InDetBCM_RawData/BCM_RawData.h"
#include "InDetBCM_RawData/BCM_RDO_Collection.h"

#include <vector>
using namespace std;

BCM_ZeroSuppression::BCM_ZeroSuppression(const std::string& name,
                                         ISvcLocator* pSvcLocator): AthAlgorithm(name, pSvcLocator),
                                                                    m_bcmContainerName("BCM_RDOs")
{
  declareProperty("BcmContainerName",     m_bcmContainerName = "BCM_RDOs");
  declareProperty("BcmOutputContainerName",     m_bcmOutputName = "BCM_CompactDOs");
}

BCM_ZeroSuppression::~BCM_ZeroSuppression()
{

}

StatusCode BCM_ZeroSuppression::initialize() {

  msg(MSG::INFO) <<  "Initialising" << endmsg;

  if (AthAlgorithm::initialize().isFailure()) {
    msg(MSG::ERROR) << "Couldn't initialize Algorithm base class." << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode BCM_ZeroSuppression::execute() {
  msg(MSG::DEBUG) << "execute()" << endmsg;

  StatusCode sc;

  //  Check for BCM RDO
  sc=evtStore()->contains<BCM_RDO_Container>(m_bcmContainerName);
  if( sc.isFailure() ) {
    msg(MSG::DEBUG) << m_bcmContainerName << " not found" << endmsg;
    return StatusCode::SUCCESS;
  }
  else {
    msg(MSG::DEBUG) << m_bcmContainerName << " container exists in StoreGate "
		    << endmsg;
  }
  
  // Retrieve BCM RDO
  m_bcmRDO = 0;
  sc=evtStore()->retrieve( m_bcmRDO,  m_bcmContainerName);
  if( sc.isFailure() || !m_bcmRDO ) {
    // There is a warning from StoreGate anyways at this point. 
    return StatusCode::SUCCESS;
  }
  else {
    msg(MSG::DEBUG) << m_bcmContainerName << " not found in StoreGate "
		    << endmsg;
  }
  
  // Create output RDO container and record it to StoreGate
  try {
    m_bcmCompactDO = new BCM_RDO_Container();
  } catch (std::bad_alloc) {
    msg(MSG::FATAL) << "Could not create a new BCM RawDataContainer!" << endmsg;
    return StatusCode::FAILURE;
  }

  sc = evtStore()->record(m_bcmCompactDO,  m_bcmOutputName);
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Container '" << m_bcmOutputName  << "' could not be registered in StoreGate" << endmsg;
    return sc;
  } else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Container '" << m_bcmOutputName << "' registered in StoreGate" << endmsg;
  }

  // check if the collection exists if in the original container and loop over it
  // and copy only those which have information in them
  BCM_RDO_Container::const_iterator BCM_RDO_itr      = m_bcmRDO->begin();
  BCM_RDO_Container::const_iterator BCM_RDO_itr_end  = m_bcmRDO->end();

  for (; BCM_RDO_itr!=BCM_RDO_itr_end; ++BCM_RDO_itr) {

    m_my_collection = new BCM_RDO_Collection();
    bool contains_hit = false;

    if ((*BCM_RDO_itr)->size() != 0){
      BCM_RDO_Collection::const_iterator RDO_element = (*BCM_RDO_itr)->begin();
      BCM_RDO_Collection::const_iterator RDO_element_last = (*BCM_RDO_itr)->end();

      for (; RDO_element != RDO_element_last; ++RDO_element){

        int bcm_pulse1width = (*RDO_element)->getPulse1Width();
        int bcm_pulse2width = (*RDO_element)->getPulse2Width();
        if ((bcm_pulse1width != 0) || (bcm_pulse2width !=0)) {
	  contains_hit = true;
	  m_my_collection->push_back((new BCM_RawData(**RDO_element)));
	}

      }//end of collection loop
    }

    if (contains_hit) {
      msg(MSG::VERBOSE) << "Container '" << m_bcmOutputName  << "' is being filled" << endmsg;
      m_bcmCompactDO->push_back(m_my_collection);
    } else {
      delete m_my_collection;
    }     
    
  }//end of container loop
  
  return StatusCode::SUCCESS;
}

StatusCode BCM_ZeroSuppression::finalize() {
  return StatusCode::SUCCESS;
}
