/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// CscThresholdClusterBuilder.cxx
#include "CscThresholdClusterBuilder.h"

#include <sstream>

#include "Gaudi/Property.h"

using std::ostringstream;
using std::string;
using std::vector;

//******************************************************************************
CscThresholdClusterBuilder::CscThresholdClusterBuilder(const std::string& aname, ISvcLocator* pSvcLocator)
  : AthAlgorithm(aname, pSvcLocator)
{
}

//******************************************************************************

// Destructor.

CscThresholdClusterBuilder::~CscThresholdClusterBuilder() {}

//******************************************************************************

StatusCode
CscThresholdClusterBuilder::initialize()
{

    ATH_MSG_DEBUG("Initializing " << name());

    // Retrieve the strip fitting tool.
    ATH_CHECK(m_cluster_builder.retrieve());
    ATH_MSG_DEBUG("Retrieved strip fitting tool " << m_cluster_builder);
    // Initialise output cluster container
    ATH_CHECK(m_pclusters.initialize());
    ATH_CHECK(m_idHelperSvc.retrieve());
    return StatusCode::SUCCESS;
}

//******************************************************************************

StatusCode
CscThresholdClusterBuilder::execute()
{

    ATH_MSG_DEBUG("Processing event ");

    // Cleanup the cluster container - ready for filling
    if (m_cluster_builder) {
        std::vector<IdentifierHash> givenIDs;
        std::vector<IdentifierHash> decodedIDs;

	//prepare output
	SG::WriteHandle<Muon::CscPrepDataContainer> wh_pclusters(m_pclusters);
	Muon::CscPrepDataContainer* object = new Muon::CscPrepDataContainer(m_idHelperSvc->cscIdHelper().module_hash_max());
	if (!wh_pclusters.isPresent()) {
	  /// record the container in storeGate
	  if (wh_pclusters.record(std::unique_ptr<Muon::CscPrepDataContainer>(object)).isFailure()) {
            ATH_MSG_ERROR("Could not record container of CSC Cluster PrepData at " << m_pclusters.key());
            return StatusCode::RECOVERABLE;
	  }
	} else {
	  ATH_MSG_DEBUG("CSC Cluster PrepData Container is already in StoreGate; nothing to do ");
	  return StatusCode::SUCCESS;
	}

        if (m_cluster_builder->getClusters(givenIDs, decodedIDs, object).isFailure()) {
	  ATH_MSG_ERROR("CSC cluster building failed");
	  return StatusCode::FAILURE;
        }
    } else {
        ATH_MSG_ERROR("No cluster builder tool initialised");
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

