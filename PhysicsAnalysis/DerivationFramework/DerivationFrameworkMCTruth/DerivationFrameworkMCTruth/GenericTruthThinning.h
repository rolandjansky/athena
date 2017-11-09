/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_GENERICTRUTHTHINNING_H
#define DERIVATIONFRAMEWORK_GENERICTRUTHTHINNING_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "DerivationFrameworkMCTruth/DecayGraphHelper.h"
#include "GaudiKernel/ToolHandle.h"

namespace ExpressionParsing {
  class ExpressionParser;
}

class IThinningSvc;

namespace DerivationFramework {

  class GenericTruthThinning : public AthAlgTool, public IThinningTool {
    public: 
      GenericTruthThinning(const std::string& t, const std::string& n, const IInterface* p);
      ~GenericTruthThinning();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode doThinning() const;

    private:
      ServiceHandle<IThinningSvc> m_thinningSvc;
      ExpressionParsing::ExpressionParser *m_vertParser;
      ExpressionParsing::ExpressionParser *m_partParser;
      mutable unsigned int m_ntotvtx, m_ntotpart, m_npassvtx, m_npasspart;
      std::string m_particlesKey, m_verticesKey, m_eventsKey;
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
