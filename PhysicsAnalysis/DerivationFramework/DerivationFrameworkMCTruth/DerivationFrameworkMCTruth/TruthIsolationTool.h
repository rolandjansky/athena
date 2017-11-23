/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_TRUTHISOLATIONTOOL_H
#define DERIVATIONFRAMEWORK_TRUTHISOLATIONTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "DerivationFrameworkMCTruth/DecayGraphHelper.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "GaudiKernel/ToolHandle.h"

namespace DerivationFramework {

  class TruthIsolationTool : public AthAlgTool, public IAugmentationTool {
    public: 
      TruthIsolationTool(const std::string& t, const std::string& n, const IInterface* p);
      ~TruthIsolationTool();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      /// Parameter: input collection key
      std::string m_isoParticlesKey;
      /// Parameter: input collection key for particles used in calculation
      std::string m_allParticlesKey;
      /// Parameter: Cone size for Isolation
      std::vector<float> m_coneSizes;
      /// Parameter: only use charged particles for iso?
      bool m_chargedOnly;
      /// Parameter: List of pdgIDs of particles to dress
      std::vector<int> m_listOfPIDs;
      /// Parameter: List of pdgIDs to exclude from cone calculation
      std::vector<int> m_excludeFromCone;
      /// Parameter: name of output variable
      std::string m_isoVarNamePrefix;

      std::vector<float> *m_coneSizesSort;
      std::vector<float> *m_coneSizes2;

      //private helper functions
      void calcIsos(const xAOD::TruthParticle* particle,
          const std::vector<const xAOD::TruthParticle*> &,
          std::vector<float>&) const;
      float calculateDeltaR2(const xAOD::IParticle *p1, float eta2, float phi2) const;
  }; 
}

#endif // DERIVATIONFRAMEWORK_TRUTHISOLATIONTOOL_H
