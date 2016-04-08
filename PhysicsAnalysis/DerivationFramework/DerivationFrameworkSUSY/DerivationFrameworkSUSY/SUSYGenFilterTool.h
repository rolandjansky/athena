/** -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @file DerivationFrameworkSUSY/SUSYGenFilterTool.h
 * @author TJ Khoo
 * @date July 2015
 * @brief tool to decorate EventInfo with quantities needed to disentangle generator filtered samples
*/


#ifndef DerivationFramework_SUSYGenFilterTool_H
#define DerivationFramework_SUSYGenFilterTool_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"

namespace DerivationFramework {

  class SUSYGenFilterTool : public AthAlgTool, public IAugmentationTool {

  public:
    SUSYGenFilterTool(const std::string& t, const std::string& n, const IInterface* p);
    ~SUSYGenFilterTool();
    StatusCode initialize();
    StatusCode finalize();
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
