/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigHLTSoftKiller: Trigger algorithm that reads CaloClusters from an
//                    incoming trigger element, defines a selection on
//                    the clusters, and outputs a new cluster collection

#include "TrigHLTJetRec/TrigHLTSoftKiller.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

TrigHLTSoftKiller::TrigHLTSoftKiller(const std::string& name, ISvcLocator* pSvcLocator)
    : HLT::FexAlgo(name, pSvcLocator)
{
    declareProperty( "output_collection_label", m_outputCollectionLabel);
}

TrigHLTSoftKiller::~TrigHLTSoftKiller()
{ }

HLT::ErrorCode TrigHLTSoftKiller::hltInitialize()
{
    
    ATH_MSG_INFO("Initializing " << name() << "...");
    
    // TODO Retrieve any needed ToolHandles here, like the SoftKiller tool

    ATH_MSG_INFO("Initialization successful");

    return HLT::OK;
}

HLT::ErrorCode TrigHLTSoftKiller::hltFinalize()
{
    ATH_MSG_INFO("Finalizing " << name() << "...");
    return HLT::OK;
}

HLT::ErrorCode TrigHLTSoftKiller::hltExecute(const HLT::TriggerElement* inputTE,
                                                   HLT::TriggerElement* outputTE)
{
    // Retrieve the input CaloCluster container from the input trigger element
    // Get a new SoftKiller-suppressed collection of CaloClusters
    // Attach the resulting collection to the output trigger element
    
    ATH_MSG_DEBUG("outputTE->getId(): " << outputTE->getId());
    ATH_MSG_DEBUG("inputTE->getId(): " << inputTE->getId());

    // Get the input container
    const xAOD::CaloClusterContainer* clusters = nullptr;
    HLT::ErrorCode status = getFeature(inputTE,clusters);
    if (status == HLT::OK)
    {
        if (clusters != nullptr)
            ATH_MSG_DEBUG("Retrieved input cluster container of size " << clusters->size());
        else
        {
            ATH_MSG_ERROR("Retrieved NULL input cluster container");
            return HLT::ERROR;
        }
    }
    else
    {
        ATH_MSG_ERROR("Failed to retrieve input cluster container");
        return HLT::ERROR;
    }



    // Apply SoftKiller and store the output in clustersSK
    const xAOD::CaloClusterContainer* clustersSK = clusters; // TODO change this to the SK cluster output
    // TODO add SK here



    // Write the resulting container
    std::string key = "";
    status = recordAndAttachFeature(outputTE,clustersSK,key,m_outputCollectionLabel);
    if (status == HLT::OK)
        ATH_MSG_DEBUG("Attached SK cluster container to output TE");
    else
        ATH_MSG_ERROR("Failed to attach SK cluster container to output TE, status " << status);
    return status;
}

