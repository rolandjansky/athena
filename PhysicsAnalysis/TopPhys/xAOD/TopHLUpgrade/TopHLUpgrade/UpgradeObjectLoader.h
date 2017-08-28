/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TOP_UPGRADEOBJECTLOADER_H_
#define _TOP_UPGRADEOBJECTLOADER_H_

#include "AsgTools/AsgTool.h"

#include "TopHLUpgrade/UpgradePerformanceFunctionsxAOD.h"
#include "TopParticleLevel/ParticleLevelEvent.h"
#include "TopParticleLevel/ObjectSelectorBase.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODCore/ShallowCopy.h"

// Forward Declarations
namespace top {
  class TopConfig;
}

namespace top {

  class UpgradeObjectLoader : public asg::AsgTool {

  public:

    /*!
     * @brief Constructor of loader tool.
     */
    UpgradeObjectLoader( const std::shared_ptr<top::TopConfig> & cfg );

    /*!
     * @brief Destructor of loader tool.
     */
    ~UpgradeObjectLoader( );

    /*!
     * @brief Loading function. Does the actual work. This function will
     *  load the truth information from the #xaodEvent and return this data
     *  transformed into a smeared level event. The data stored in the
     *  SmearedLevel is owned by the truth event loader.
     * @param xaodEvent The input data event object.
     * @return ParticleLevelEvent object.
     */
    ParticleLevelEvent load();
    
    /*!
     * @brief Function that can be used to test whether the UpgradeObjectLoader
     *  is active. The loader is set to inactive if the names of the truth
     *  collections are unavailable. This is determined at construction.
     */
    bool active() const { return m_active; }
    
  private:

    // The global config object
    const std::shared_ptr<top::TopConfig> & m_config;

    // pointer to smearing function object
    std::unique_ptr<UpgradePerformanceFunctionsxAOD> m_upgrade;

    // Flag denoting whether the loader tool is active. Will be set by the
    // constructor and remains unchanged afterwards.
    const bool m_active;

    // muons
    std::unique_ptr<xAOD::TruthParticleContainer> m_muons;
    std::unique_ptr<xAOD::ShallowAuxContainer> m_muonsShallowAux;
    std::unique_ptr<xAOD::TruthParticleContainer> m_selectedMuons;

    // electrons
    std::unique_ptr<xAOD::TruthParticleContainer> m_electrons;
    std::unique_ptr<xAOD::ShallowAuxContainer> m_electronsShallowAux;
    std::unique_ptr<xAOD::TruthParticleContainer> m_selectedElectrons;

    // jets
    std::unique_ptr<xAOD::JetContainer> m_jets;
    std::unique_ptr<xAOD::JetAuxContainer> m_jetsAux;

    // MET
    std::unique_ptr<xAOD::MissingETContainer> m_metCont;
    std::unique_ptr<xAOD::ShallowAuxContainer> m_metShallowAux;

    // Electron selector tool
    std::unique_ptr<ObjectSelectorBase<xAOD::TruthParticle> > m_objectSelector_Electron;

    // Muon selector tool
    std::unique_ptr<ObjectSelectorBase<xAOD::TruthParticle> > m_objectSelector_Muon;

  };//class UpgradeObjectLoader

}//namespace top

#endif //_TOP_UPGRADEOBJECTLOADER_H_
