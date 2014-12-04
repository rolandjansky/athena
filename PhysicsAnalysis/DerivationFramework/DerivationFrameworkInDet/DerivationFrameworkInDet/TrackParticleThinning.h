/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackParticleThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TRACKPARTICLETHINNING_H
#define DERIVATIONFRAMEWORK_TRACKPARTICLETHINNING_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace ExpressionParsing {
  class ExpressionParser;
}

class IThinningSvc;

namespace DerivationFramework {

  class TrackParticleThinning : public AthAlgTool, public IThinningTool {
    public: 
      TrackParticleThinning(const std::string& t, const std::string& n, const IInterface* p);
      ~TrackParticleThinning();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode doThinning() const;

    private:
      ServiceHandle<IThinningSvc> m_thinningSvc;
      std::string m_expression;
      ExpressionParsing::ExpressionParser *m_parser;
      mutable unsigned int m_ntot, m_npass;
      std::string m_inDetSGKey, m_selectionString;
      bool m_and;
  }; 
}

#endif // DERIVATIONFRAMEWORK_TRACKPARTICLETHINNING_H
