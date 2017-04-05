/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigHLTSoftKiller: Trigger algorithm that reads CaloClusters from an
//                    incoming trigger element, defines a selection on
//                    the clusters, and outputs a new cluster collection

#include "TrigHLTJetRec/TrigHLTSoftKiller.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODBase/IParticleContainer.h"

TrigHLTSoftKiller::TrigHLTSoftKiller(const std::string& name, ISvcLocator* pSvcLocator)
    : HLT::FexAlgo(name, pSvcLocator)
{
    declareProperty( "output_collection_label", m_outputCollectionLabel);
    declareProperty("skclustModSeqTool",m_skclustModSeqTool);
}

TrigHLTSoftKiller::~TrigHLTSoftKiller()
{ }

HLT::ErrorCode TrigHLTSoftKiller::hltInitialize()
{
    
    ATH_MSG_INFO("Initializing " << name() << "...");
    
   StatusCode sc = m_skclustModSeqTool.retrieve();

   if (sc.isSuccess())
       ATH_MSG_INFO("Retrieved skclustModSeqTool: " << m_skclustModSeqTool->name());
    else
    {
     	ATH_MSG_ERROR("Failed to retrieve the skclustModSeqTool: " << m_skclustModSeqTool->name());
        return HLT::ERROR;
    }

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
    const xAOD::CaloClusterContainer* inputclusters = nullptr;
    HLT::ErrorCode status = getFeature(inputTE,inputclusters);
    if (status == HLT::OK)
    {
        if (inputclusters != nullptr)
            ATH_MSG_DEBUG("Retrieved input cluster container of size " << inputclusters->size());
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

    JetConstituentModSequence *skclustModSeqTool = const_cast<JetConstituentModSequence*>(dynamic_cast<const JetConstituentModSequence*>(&*m_skclustModSeqTool));
    const xAOD::IParticleContainer* IP_inputclusters = dynamic_cast<const xAOD::IParticleContainer*> (inputclusters);
    skclustModSeqTool->setInputClusterCollection(IP_inputclusters);
    //skclustModSeqTool->setInputClusterCollection(inputclusters);
    ATH_MSG_INFO("Input cluster container: " << inputclusters);
    int process_status = skclustModSeqTool->execute();
    const xAOD::CaloClusterContainer* outputclusters = dynamic_cast<const xAOD::CaloClusterContainer*>(skclustModSeqTool->getOutputClusterCollection()); 
    ATH_MSG_INFO("Output cluster container after: " << outputclusters);
    
    if (process_status == 0)
    {
        if (outputclusters != nullptr)
            ATH_MSG_DEBUG("Processed cluster container of size " << outputclusters->size());
        else
        {
            ATH_MSG_ERROR("SoftKillerWeightTool returned NULL input cluster container");
            return HLT::ERROR;
        }
    }
    else
    {
        ATH_MSG_ERROR("Failed to retrieve processed cluster container");
        return HLT::ERROR;
    }
    
    ////////// FOR DEBUGGING PURPOSES
    //SG::AuxElement::ConstAccessor<float> weightAcc("PUWeight"); // Handle for PU weighting here
    //for (size_t icl = 0; icl < outputclusters->size(); ++icl)
    //    if (weightAcc(*(outputclusters->at(icl))) < 1.e-6)
    //        ATH_MSG_INFO("Cluster SK weight: " << weightAcc(*(outputclusters->at(icl))) << ", pT = " << outputclusters->at(icl)->pt());

    ATH_MSG_DEBUG("writing results");


    // Write the resulting container
    std::string key = "";
    status = recordAndAttachFeature(outputTE,outputclusters,key,m_outputCollectionLabel);
    if (status == HLT::OK)
        ATH_MSG_DEBUG("Attached SK cluster container to output TE");
    else
        ATH_MSG_ERROR("Failed to attach SK cluster container to output TE, status " << status);
    return status;
}

