/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DiTauTrackParticleThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_DITAUTRACKPARTICLETHINNING_H
#define DERIVATIONFRAMEWORK_DITAUTRACKPARTICLETHINNING_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInDet/TracksInCone.h"

namespace ExpressionParsing {
  class ExpressionParser;
}

class IThinningSvc;

namespace DerivationFramework {

  class DiTauTrackParticleThinning : public AthAlgTool, public IThinningTool {
    public: 
      DiTauTrackParticleThinning(const std::string& t, const std::string& n, const IInterface* p);
      ~DiTauTrackParticleThinning();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode doThinning() const;

    private:
      ServiceHandle<IThinningSvc> m_thinningSvc;
      mutable unsigned int m_ntot, m_npass;
      std::string m_ditauSGKey, m_inDetSGKey, m_selectionString;
      bool m_and;
      ExpressionParsing::ExpressionParser *m_parser;
  }; 
}

#endif // DERIVATIONFRAMEWORK_DITAUTRACKPARTICLETHINNING_H
