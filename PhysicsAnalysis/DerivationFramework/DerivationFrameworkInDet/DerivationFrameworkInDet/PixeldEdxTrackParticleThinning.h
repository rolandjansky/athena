/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackParticleThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_PIXELDEDXTRACKPARTICLETHINNING_H
#define DERIVATIONFRAMEWORK_PIXELDEDXTRACKPARTICLETHINNING_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "GaudiKernel/ToolHandle.h"

#include <string>
#include <vector>

namespace ExpressionParsing {
  class ExpressionParser;
}

class IThinningSvc;

namespace DerivationFramework {

  class PixeldEdxTrackParticleThinning : public AthAlgTool, public IThinningTool {
    public: 
      PixeldEdxTrackParticleThinning(const std::string& t, const std::string& n, const IInterface* p);
      ~PixeldEdxTrackParticleThinning();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode doThinning() const;

    private:
      //Main thinning service
      ServiceHandle<IThinningSvc> m_thinningSvc;

      //Expression for object thinning selection
      std::string m_expression;
      ExpressionParsing::ExpressionParser *m_parser;
      std::string m_selectionString;

      //Counters and keys for xAOD::TrackParticle container
      mutable unsigned int m_ntot, m_npass;
      std::string m_inDetSGKey;
    
      double m_unprescalePtCut;
      double m_globalScale;
      double m_d0SignifCut;
      double m_z0Cut;
      double m_etaCut;

      //logic
      bool m_and;

    mutable std::vector<unsigned long long> counter;
    mutable std::vector<unsigned long long> counter_picked;
    
  }; 
}

#endif // DERIVATIONFRAMEWORK_PIXELDEDXTRACKPARTICLETHINNING_H
