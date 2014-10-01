/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_showerdeconstruction_header
#define jetsubstructureutils_showerdeconstruction_header

#include <vector>

#include "xAODJet/Jet.h"

namespace fastjet {
  class PseudoJet;
}

class AnalysisParameters;
namespace Deconstruction {
  class WDecayModel;
  class TopGluonModel;
  class BackgroundModel;
  class ISRModel;
  class Deconstruct;
}

namespace JetSubStructureUtils {

  class ShowerDeconstruction { 
    public:
      enum SignalModel {
        WDecayModel,
        TopGluonModel
      };
      ShowerDeconstruction(SignalModel signalModel);
      virtual ~ShowerDeconstruction();

      virtual double result(const xAOD::Jet &jet);
      virtual double result(const fastjet::PseudoJet &jet, const float R);

    private:
      AnalysisParameters *m_param;
      Deconstruction::WDecayModel *m_WModel;
      Deconstruction::TopGluonModel *m_topModel;
      Deconstruction::BackgroundModel *m_bkgModel;
      Deconstruction::ISRModel *m_isrModel;
      Deconstruction::Deconstruct *m_deconstruct;
  };
}

#endif
