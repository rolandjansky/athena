/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TCCTrackParticleThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TCCTRACKPARTICLETHINNING_H
#define DERIVATIONFRAMEWORK_TCCTRACKPARTICLETHINNING_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace ExpressionParsing {
  class ExpressionParser;
}

class IThinningSvc;

namespace DerivationFramework {

  class TCCTrackParticleThinning : public AthAlgTool, public IThinningTool {
    public: 
      TCCTrackParticleThinning(const std::string& t, const std::string& n, const IInterface* p);
      ~TCCTrackParticleThinning();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode doThinning() const;

    private:
      ServiceHandle<IThinningSvc> m_thinningSvc;
      mutable unsigned int m_ntot, m_npass, m_ntotCC, m_npassCC, m_ntotTCC, m_npassTCC;
      std::string m_tccSGKey, m_inDetSGKey, m_calCTCSGKey, m_oCalCTCSGKey, m_jetSGKey/*, m_selectionString*/;
      bool m_and, m_thinO;
//       ExpressionParsing::ExpressionParser *m_parser;
  }; 
}

#endif // DERIVATIONFRAMEWORK_TCCTRACKPARTICLETHINNING_H
