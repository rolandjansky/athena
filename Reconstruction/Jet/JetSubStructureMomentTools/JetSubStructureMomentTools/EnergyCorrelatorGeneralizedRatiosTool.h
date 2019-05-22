/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_energycorrelatorgeneralizedratiostool_header
#define jetsubstructuremomenttools_energycorrelatorgeneralizedratiostool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"
#include "JetSubStructureMomentTools/ECFHelper.h"

class EnergyCorrelatorGeneralizedRatiosTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(EnergyCorrelatorGeneralizedRatiosTool, IJetModifier)
    
    public:
      // Constructor and destructor
      EnergyCorrelatorGeneralizedRatiosTool(std::string name);

      StatusCode initialize();

      int modifyJet(xAOD::Jet &jet) const;

    private:
      bool m_doN3;
      bool m_doLSeries;
      std::vector<float> m_betaVals;
      std::vector<float> betaVals; // Local vector for cleaned up inputs
      bool m_doDichroic;

};

#endif
