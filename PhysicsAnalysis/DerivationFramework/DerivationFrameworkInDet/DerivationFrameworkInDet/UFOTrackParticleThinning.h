/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// UFOTrackParticleThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_UFOTRACKPARTICLETHINNING_H
#define DERIVATIONFRAMEWORK_UFOTRACKPARTICLETHINNING_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace ExpressionParsing {
  class IProxyLoader;
  class ExpressionParser;
  class MultipleProxyLoader;
}

class IThinningSvc;

namespace DerivationFramework {

  class UFOTrackParticleThinning : public AthAlgTool, public IThinningTool {
    public: 
      UFOTrackParticleThinning(const std::string& t, const std::string& n, const IInterface* p);
      ~UFOTrackParticleThinning();
      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;
      virtual StatusCode doThinning() const override;

    private:
      ServiceHandle<IThinningSvc> m_thinningSvc;
      mutable unsigned int m_ntot, m_npass, m_ntotCC, m_npassCC, m_ntotUFO, m_npassUFO;
      std::string m_ufoSGKey, m_inDetSGKey, m_jetSGKey, m_selectionString, m_PFOSGKey;
      bool m_and;
      std::vector< std::string > m_addPFOSGKey;
      std::vector<std::unique_ptr<ExpressionParsing::IProxyLoader> > m_cleanup;
      std::unique_ptr<ExpressionParsing::MultipleProxyLoader>        m_proxyLoaders;
      std::unique_ptr<ExpressionParsing::ExpressionParser>           m_parser;
  }; 
}

#endif // DERIVATIONFRAMEWORK_UFOTRACKPARTICLETHINNING_H
