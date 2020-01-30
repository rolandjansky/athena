/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Tool for decorating events with truth related quantities, in which the full truth record is assumed to still be available
// Ported originally from code in the HWWPhysicsxAODMaker by D.Shope (david.richard.shope@cern.ch):
// https://gitlab.cern.ch/atlas-physics/higgs/hww/HWWPhysicsxAODMaker

#ifndef DerivationFrameworkHiggs_HIGG3TruthDecorator_H
#define DerivationFrameworkHiggs_HIGG3TruthDecorator_H

#include <string>
#include <vector>
#include <algorithm>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

// Additional tool includes
#include "GammaORTools/IVGammaORTool.h"

// EDM includes
#include "xAODTruth/TruthParticleFwd.h"
#include "xAODBase/IParticle.h"
#include "xAODMissingET/MissingETContainer.h"

namespace DerivationFramework {

  class HIGG3TruthDecorator : public AthAlgTool, public IAugmentationTool {

  public:
    /** Constructor with parameters */
    HIGG3TruthDecorator(const std::string& t, const std::string& n, const IInterface* p);

    /** Destructor */
    ~HIGG3TruthDecorator();

    // Athena algtool's Hooks
    StatusCode initialize();
    StatusCode finalize();

    virtual StatusCode addBranches() const;

  private:

    /// @name Private helper functions

    /// Check if a truth partice decays to itself
    bool decayIntoItself(const xAOD::TruthParticle* part, int status=-1)  const;

    /// See if the truth particle is the last of its kind
    const xAOD::TruthParticle* lastOfKind(const xAOD::TruthParticle* part)  const;

    /// See if the truth particle is the first of its kind
    const xAOD::TruthParticle* firstOfKind(const xAOD::TruthParticle* part)  const;

    /// Helper to recursively print info of a decay chain
    void printRecursively(const xAOD::TruthParticle* part, std::string preVal="")  const;

    /// Check for particles from W bosons
    bool isFromW( const xAOD::TruthParticle* part, const std::vector<const xAOD::TruthParticle*>& Wlist)  const;

    /// Sum the four-momenta of all daughter neutrinos of the given particle
    TLorentzVector sumDaughterNeutrinos(const xAOD::TruthParticle *part)  const;

    /// Check if the given particle overlaps in DeltaR with any of the list
    bool checkOverlap(const xAOD::IParticle* part, std::vector<const xAOD::TruthParticle* > list)  const;

    /// Check if the given particle overlaps in DeltaR with any of the list
    bool checkOverlap(const xAOD::IParticle* part, std::vector<TLorentzVector > list)  const;

    ///Calculate TruthMT
    double calculateMT(const xAOD::TruthParticle *lep1, const xAOD::TruthParticle *lep2, const xAOD::MissingET* met)  const;
    /// @}

  private:

    /// @name The properties that can be defined via the python job options
    /// @{

    std::string m_inElContName;
    std::string m_inMuContName;
    std::string m_inEMTopoJetContName;
    std::string m_inPFlowJetContName;

    /// The minimum pt threshold for a truth-jet to be considered for matching
    double m_truthJetPtMin;

    /// The maximum deltaR (using rapidity and NOT eta) distance for a successful truth match
    double m_maxDeltaR;

    bool m_isSherpa;
    bool m_isPowPy8EvtGen;

    /// @}

    ToolHandle<IVGammaORTool> m_GamORTool;

    enum WWType{
      NOWW=0,
      TAUTAU,
      TAUE,
      TAUM,
      EE,
      EM,
      MM
    };

  };

}

#endif