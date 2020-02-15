/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

      virtual StatusCode initialize() override;

      int modifyJet(xAOD::Jet &jet) const override;

    private:
      std::vector<float> m_betaVals;
      std::vector<float> m_cleaned_betaVals; // Local vector for cleaned up inputs

};

#endif
