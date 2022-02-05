/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_XAODTRUTHPARTICLESLIMMERLIGHTLEPTONS_H
#define GENERATORFILTERS_XAODTRUTHPARTICLESLIMMERLIGHTLEPTONS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthMetaDataContainer.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

/// @brief Algorithm to skim the xAOD truth particle container for xAOD electron&muon filter
///
/// This algorithm is used to copy and skim the particles from the xAOD TruthParticles container,
/// keeping just relevant taus from the event.
/// The design of this class heavily mirrors the DerivationFramework::TruthCollectionMaker.
///
/// @author Jeff Dandoy <Jeff.Dandoy@cern.ch>
class xAODTruthParticleSlimmerLightLepton : public AthReentrantAlgorithm
{
public:
    /// Regular algorithm constructor
    xAODTruthParticleSlimmerLightLepton(const std::string &name, ISvcLocator *svcLoc);
    /// Function initialising the algorithm
    virtual StatusCode initialize();
    /// Function executing the algorithm
    virtual StatusCode execute(const EventContext& context) const;

private:
    /// The key for the output xAOD truth containers
    SG::ReadHandleKey<xAOD::TruthEventContainer> m_xaodTruthEventContainerNameReadHandleKey
    {this,"xAODTruthEventContainerName","TruthEvents","Name of Truth Events container"};
    SG::WriteHandleKey<xAOD::TruthParticleContainer> m_xaodTruthParticleContainerNameLightLeptonKey
    {this, "xAODTruthParticleContainerNameLightLepton","TruthLightLeptons","Name of Truth Light Leptons contatiner from the slimmer"};
    


}; // class xAODTruthParticleSlimmerLightLepton

#endif //GENERATORFILTERS_XAODTRUTHPARTICLESLIMMERLIGHTLEPTONS_H