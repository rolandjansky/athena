/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// Filename: ParticleLevelLoader.h
// Description:
// Author: Fabian Wilk
// Created: Sun Feb 22 13:09:33 2015

#ifndef _TOP_PARTICLELEVEL_EVENTLOADER_H_
#define _TOP_PARTICLELEVEL_EVENTLOADER_H_

#include <memory>

#include "TopParticleLevel/ObjectSelectorBase.h"
#include "TopParticleLevel/ParticleLevelRCJetObjectLoader.h"

#include "AsgTools/AsgTool.h"

#include "xAODCore/ShallowCopy.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"

// Forward Declarations
namespace top {
  class ParticleLevelEvent;
  class TopConfig;
}

namespace top {
  /*!
   * @brief Loading tool which creates a particle level event object.
   *
   * After creation of the loader tool, it will be in of of two states: either
   * it is "active" or not (can be tested using the active() member function).
   * If the tool is not active, then the load function is a no-op, it will
   * return an "empty" ParticleLevelEvent object. The active() member function
   * allows for the enclosing (i.e. calling) program to conditionally branch
   * into computing intensive parts depending on whether particle level data
   * is available or not. The loader tool is configured to be non-"active" if
   * the input data is not monte carlo or any of the required collection names
   * are empty respectively None
   */
  class ParticleLevelLoader final: public asg::AsgTool {
  public:
    /*!
     * @brief Constructor of the loader tool.
     */
    ParticleLevelLoader(const std::shared_ptr<top::TopConfig>& cfg);

    /*!
     * @brief Destructor of the loader tool.
     */
    virtual ~ParticleLevelLoader();

    /*!
     * @brief Loading function. Does the actual work. This function will
     *  load the truth information from the #xaodEvent and return this data
     *  transformed into a truth level event. The data stored in the
     *  ParticleLevel is owned by the truth event loader.
     * @param xaodEvent The input data event object.
     * @return ParticleLevelEvent object.
     */
    ParticleLevelEvent load();

    /*!
     * @brief Function that can be used to test whether the ParticleLevelLoader
     *  is active. The loader is set to inactive if the names of the truth
     *  collections are unavailable. This is determined at construction.
     */
    bool active() const {return m_active;}
  protected:
    bool loadDressedLeptons(const xAOD::TruthParticleContainer& input,
                            std::unique_ptr<xAOD::TruthParticleContainer>& store,
                            std::unique_ptr<xAOD::ShallowAuxContainer>& storeAux) const;

    bool isDressingPhoton(const xAOD::TruthParticle& photon,
                          const xAOD::TruthParticleContainer& dressedParticles,
                          const float dressingCone = 0.1) const;
  private:
    // The global config object
    const std::shared_ptr<top::TopConfig>& m_config;

    // The tools used for electron, muon, and jet object selection. The
    // basic idea is to allow exchanging these tools for custom ones,
    // however, this is not implemented (yet).
    std::unique_ptr<ObjectSelectorBase<xAOD::TruthParticle> > m_objectSelector_Electron;
    std::unique_ptr<ObjectSelectorBase<xAOD::TruthParticle> > m_objectSelector_Muon;
    std::unique_ptr<ObjectSelectorBase<xAOD::TruthParticle> > m_objectSelector_Photon;
    std::unique_ptr<ObjectSelectorBase<xAOD::Jet> > m_objectSelector_Jet;
    std::unique_ptr<ObjectSelectorBase<xAOD::Jet> > m_objectSelector_LargeRJet;
    std::unique_ptr<ObjectSelectorBase<xAOD::TruthParticle> > m_objectSelector_Tau;
    std::unique_ptr<ParticleLevelRCJetObjectLoader> m_particleLevelRCJetObjectLoader;
    std::map<std::string, std::unique_ptr<ParticleLevelRCJetObjectLoader> > m_particleLevelVarRCJetObjectLoader;
    std::vector<std::string> m_VarRCJetRho;
    std::vector<std::string> m_VarRCJetMassScale;
  private:
    // The dressed leptons (shallow copies of the input containers)
    std::unique_ptr<xAOD::TruthParticleContainer> m_electronsDressed;
    std::unique_ptr<xAOD::ShallowAuxContainer> m_electronsDressedAux;

    std::unique_ptr<xAOD::TruthParticleContainer> m_muonsDressed;
    std::unique_ptr<xAOD::ShallowAuxContainer> m_muonsDressedAux;

    std::unique_ptr<xAOD::TruthParticleContainer> m_goodElectrons;
    std::unique_ptr<xAOD::TruthParticleAuxContainer> m_goodElectronsAux;

    std::unique_ptr<xAOD::TruthParticleContainer> m_goodMuons;
    std::unique_ptr<xAOD::TruthParticleAuxContainer> m_goodMuonsAux;
    
    std::unique_ptr<xAOD::TruthParticleContainer> m_goodSoftMuons;
    std::unique_ptr<xAOD::TruthParticleAuxContainer> m_goodSoftMuonsAux;

    std::unique_ptr<xAOD::TruthParticleContainer> m_goodPhotons;
    std::unique_ptr<xAOD::TruthParticleAuxContainer> m_goodPhotonsAux;

    std::unique_ptr<xAOD::JetContainer> m_goodJets;
    std::unique_ptr<xAOD::JetAuxContainer> m_goodJetsAux;

    std::unique_ptr<xAOD::JetContainer> m_goodLargeRJets;
    std::unique_ptr<xAOD::JetAuxContainer> m_goodLargeRJetsAux;

    std::unique_ptr<xAOD::TruthParticleContainer> m_goodTaus;
    std::unique_ptr<xAOD::TruthParticleAuxContainer> m_goodTausAux;

    // Flag denoting whether the loader tool is active. Will be set by the
    // constructor and remains unchanged afterwards.
    const bool m_active;
  };
}

#endif /* _TOP_PARTICLELEVEL_EVENTLOADER_H_ */
