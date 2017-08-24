/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @file GenFilterTool.h
 * @author TJ Khoo
 * @date July 2015
 * @brief tool to decorate EventInfo with quantities needed to disentangle generator filtered samples
*/

#ifndef DerivationFrameworkMCTruth_GenFilterTool_H
#define DerivationFrameworkMCTruth_GenFilterTool_H

// Base classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

// Tool handle for the MC truth classifier
#include "GaudiKernel/ToolHandle.h"

// EDM include -- typedef, so has to be included
#include "xAODTruth/TruthParticleContainer.h"

// Defs for the particle origin
#include "MCTruthClassifier/MCTruthClassifierDefs.h"

// STL includes
#include <string>

class IMCTruthClassifier;

namespace DerivationFramework {

  class GenFilterTool : public AthAlgTool, public IAugmentationTool {

  public:
    GenFilterTool(const std::string& t, const std::string& n, const IInterface* p);
    ~GenFilterTool();
    virtual StatusCode addBranches() const;

    StatusCode getGenFiltVars(const xAOD::TruthParticleContainer* tpc, float& genFiltHT, float& genFiltMET) const;

    bool isPrompt( const xAOD::TruthParticle* tp ) const;
    MCTruthPartClassifier::ParticleOrigin getPartOrigin(const xAOD::TruthParticle* tp) const;

  private:

    std::string m_eventInfoName;
    std::string m_mcName;
    std::string m_truthJetsName;

    float m_MinJetPt;  //!< Min pT for the truth jets
    float m_MaxJetEta; //!< Max eta for the truth jets
    float m_MinLepPt;  //!< Min pT for the truth leptons
    float m_MaxLepEta; //!< Max eta for the truth leptons
    int m_SimBarcodeOffset; //!< G4 particle barcode offset value (Particles having a barcode greater than this value are defined to be G4 particles)

    mutable std::map<const xAOD::TruthParticle*,MCTruthPartClassifier::ParticleOrigin> m_originMap;
    ToolHandle<IMCTruthClassifier> m_classif;

  }; /// class

} /// namespace

#endif
