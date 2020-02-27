/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenericTruthThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_GENERICTRUTHTHINNING_H
#define DERIVATIONFRAMEWORK_GENERICTRUTHTHINNING_H

#include <string>
#include <atomic>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "DerivationFrameworkMCTruth/DecayGraphHelper.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "StoreGate/ThinningHandleKey.h"
#include "GaudiKernel/ToolHandle.h"

namespace ExpressionParsing {
  class ExpressionParser;
}


namespace DerivationFramework {

  class GenericTruthThinning : public extends<AthAlgTool, IThinningTool> {
    public: 
      GenericTruthThinning(const std::string& t, const std::string& n, const IInterface* p);
      virtual ~GenericTruthThinning();
      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;
      virtual StatusCode doThinning() const override;

    private:
      //ExpressionParsing::ExpressionParser *m_vertParser;
      ExpressionParsing::ExpressionParser *m_partParser;
      mutable std::atomic<unsigned int> m_ntotvtx, m_ntotpart, m_npassvtx, m_npasspart;
      StringProperty m_streamName
        { this, "StreamName", "", "Name of the stream being thinned" };
      SG::ThinningHandleKey<xAOD::TruthParticleContainer> m_particlesKey
        { this, "ParticlesKey", "TruthParticles", "" };
      SG::ThinningHandleKey<xAOD::TruthVertexContainer> m_verticesKey
        { this, "VerticesKey", "TruthVertices", "" };
      std::string m_eventsKey;
      std::string m_partString;
      //std::string m_vtxString;
      bool m_preserveDescendants;
      bool m_preserveGeneratorDescendants;
      bool m_preserveAncestors;
      bool m_tauHandling;
      int m_geantOffset;
  }; 
}

#endif // DERIVATIONFRAMEWORK_GENERICTRUTHTHINNING_H
