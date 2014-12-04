/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTrackParticleThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_MUONTRACKPARTICLETHINNING_H
#define DERIVATIONFRAMEWORK_MUONTRACKPARTICLETHINNING_H

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

  class MuonTrackParticleThinning : public AthAlgTool, public IThinningTool {
    public: 
      MuonTrackParticleThinning(const std::string& t, const std::string& n, const IInterface* p);
      ~MuonTrackParticleThinning();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode doThinning() const;

    private:
      ServiceHandle<IThinningSvc> m_thinningSvc;
      mutable unsigned int m_ntot, m_npass;
      std::string m_muonSGKey;
      std::string m_inDetSGKey; 
      std::string m_selectionString; 
      float m_coneSize;	
      bool m_and;
      ExpressionParsing::ExpressionParser *m_parser;
  }; 
}

#endif // DERIVATIONFRAMEWORK_MUONTRACKPARTICLETHINNING_H
