/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/StatusCode.h"

#include "BCM_ZeroSuppression/BCM_ZeroSuppression.h"

#include "InDetBCM_RawData/BCM_RawData.h"
#include "InDetBCM_RawData/BCM_RDO_Collection.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include <vector>
using namespace std;

BCM_ZeroSuppression::BCM_ZeroSuppression(const std::string& name,
                                         ISvcLocator* pSvcLocator): AthAlgorithm(name, pSvcLocator)
{
  declareProperty("BcmContainerName",     m_bcmContainerName = "BCM_RDOs");
  declareProperty("BcmOutputContainerName",     m_bcmOutputName = "BCM_CompactDOs");
}

BCM_ZeroSuppression::~BCM_ZeroSuppression()
= default;

StatusCode BCM_ZeroSuppression::initialize() {
  ATH_CHECK( m_bcmContainerName.initialize() );
  ATH_CHECK( m_bcmOutputName.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode BCM_ZeroSuppression::execute() {
  // Retrieve BCM RDO
  SG::ReadHandle<BCM_RDO_Container> bcm_rdo(m_bcmContainerName);
  if( !bcm_rdo.isValid() ) {
    // There is a warning from StoreGate anyways at this point.
    // @TODO return failure instead ?
    return StatusCode::SUCCESS;
  }

  // Create output RDO container and record it to StoreGate
  SG::WriteHandle<BCM_RDO_Container> bcm_compact_DO(m_bcmOutputName);
  if (bcm_compact_DO.record(std::make_unique<BCM_RDO_Container>()).isFailure()) {
    ATH_MSG_FATAL( "Container '" << m_bcmOutputName  << "' could not be registered in StoreGate");
    return StatusCode::FAILURE;
  }

  // check if the collection exists if in the original container and loop over it
  // and copy only those which have information in them
  for (const BCM_RDO_Collection *BCM_RDO: *bcm_rdo) {

    std::unique_ptr<BCM_RDO_Collection> my_collection(std::make_unique<BCM_RDO_Collection>() );

    for (const BCM_RawData *RDO_element: *BCM_RDO) {

      int bcm_pulse1width = RDO_element->getPulse1Width();
      int bcm_pulse2width = RDO_element->getPulse2Width();
      if ((bcm_pulse1width != 0) || (bcm_pulse2width !=0)) {
	  my_collection->push_back((new BCM_RawData(*RDO_element)));
      }

    }

    if (!my_collection->empty()) {
      ATH_MSG_VERBOSE("Container '" << m_bcmOutputName.key()  << "' is being filled");
      bcm_compact_DO->push_back(my_collection.release());
    }

  }//end of container loop

  return StatusCode::SUCCESS;
}

StatusCode BCM_ZeroSuppression::finalize() {
  return StatusCode::SUCCESS;
}
