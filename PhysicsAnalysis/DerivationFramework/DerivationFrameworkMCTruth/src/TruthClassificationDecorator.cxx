/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TruthClassificationDecorator.cxx
// Author: James Catmore (James.Catmore@cern.ch)
// Removes all truth particles/vertices which do not pass a user-defined cut

#include "DerivationFrameworkMCTruth/TruthClassificationDecorator.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::TruthClassificationDecorator::TruthClassificationDecorator(const std::string& t,
                                                                  const std::string& n,
                                                                  const IInterface* p ) :
AthAlgTool(t,n,p),
m_ntotpart(0),
m_particlesKey("TruthParticles"),
m_classifier("MCTruthClassifier/MCTruthClassifier")
{
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("ParticlesKey", m_particlesKey);
    declareProperty("MCTruthClassifier", m_classifier);
}

// Destructor
DerivationFramework::TruthClassificationDecorator::~TruthClassificationDecorator() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::TruthClassificationDecorator::initialize()
{
    ATH_MSG_VERBOSE("initialize() ...");
    ATH_CHECK(m_classifier.retrieve());

    if (m_particlesKey=="" /*|| m_verticesKey==""*/) {
        ATH_MSG_FATAL("No truth particle collection provided to decorate!");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Decorating " << m_particlesKey << " with classification information");}
    
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::TruthClassificationDecorator::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    //ATH_MSG_INFO("Processed "<< m_ntotvtx <<" truth vertices, "<< m_npassvtx << " were retained ");
    ATH_MSG_INFO("Processed and decorated "<< m_ntotpart <<" truth particles");
    return StatusCode::SUCCESS;
}

// Selection and collection creation
StatusCode DerivationFramework::TruthClassificationDecorator::addBranches() const
{
    
    // Retrieve truth collections
    const xAOD::TruthParticleContainer* importedTruthParticles;
    if (evtStore()->retrieve(importedTruthParticles,m_particlesKey).isFailure()) {
        ATH_MSG_ERROR("No TruthParticle collection with name " << m_particlesKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    unsigned int nParticles = importedTruthParticles->size();
    m_ntotpart += nParticles;
    
    // Set up decorators
    SG::AuxElement::Decorator< ElementLink<xAOD::TruthParticleContainer> > linkDecorator("originalTruthParticle");
    SG::AuxElement::Decorator< unsigned int > originDecorator("classifierParticleOrigin");
    SG::AuxElement::Decorator< unsigned int > typeDecorator("classifierParticleType");
    SG::AuxElement::Decorator< unsigned int > outcomeDecorator("classifierParticleOutCome");

    for (unsigned int i=0; i<nParticles; ++i) {
#ifdef MCTRUTHCLASSIFIER_CONST
        IMCTruthClassifier::Info info;
        std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> classification = 
          m_classifier->particleTruthClassifier((*importedTruthParticles)[i], &info);
        unsigned int particleOutCome = info.particleOutCome;
#else
        std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> classification = 
        m_classifier->particleTruthClassifier((*importedTruthParticles)[i]);
        unsigned int particleOutCome = m_classifier->getParticleOutCome();
#endif
        unsigned int particleType = classification.first;
        unsigned int particleOrigin = classification.second;
        typeDecorator(*((*importedTruthParticles)[i])) = particleType;
        originDecorator(*((*importedTruthParticles)[i])) = particleOrigin;
        outcomeDecorator(*((*importedTruthParticles)[i])) = particleOutCome;  
    }

    return StatusCode::SUCCESS;
}

