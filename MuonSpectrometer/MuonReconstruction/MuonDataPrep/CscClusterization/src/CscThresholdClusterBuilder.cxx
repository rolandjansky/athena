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
    : AthAlgorithm(aname, pSvcLocator),
      m_cluster_builder("CscThresholdClusterBuilderTool/CscThresholdClusterBuilderTool")
{

    declareProperty("cluster_builder", m_cluster_builder);
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
    if (m_cluster_builder.retrieve().isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve strip fitting tool " << m_cluster_builder);
        return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Retrieved strip fitting tool " << m_cluster_builder);
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

        if (m_cluster_builder->getClusters(givenIDs, decodedIDs).isFailure()) {
            ATH_MSG_ERROR("Cannot record CSC Cluster Container ");
            return StatusCode::FAILURE;
        }
    } else {
        ATH_MSG_ERROR("Initialization failed");
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

//******************************************************************************

StatusCode
CscThresholdClusterBuilder::finalize()
{
    ATH_MSG_VERBOSE("Finalizing " << name());
    //  m_pclusters->release();
    return StatusCode::SUCCESS;
}

//******************************************************************************
