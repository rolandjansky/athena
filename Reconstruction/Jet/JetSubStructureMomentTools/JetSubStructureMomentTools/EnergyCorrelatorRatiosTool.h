/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_energycorrelatorratiostool_header
#define jetsubstructuremomenttools_energycorrelatorratiostool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"
#include "JetSubStructureMomentTools/ECFHelper.h"

class EnergyCorrelatorRatiosTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(EnergyCorrelatorRatiosTool, IJetModifier)
    
    public:
      // Constructor and destructor
      EnergyCorrelatorRatiosTool(std::string name);
     
      virtual StatusCode initialize() override;

      int modifyJet(xAOD::Jet &jet) const override;

    private:
      bool m_doC3;
      std::vector<float> m_betaVals;
      std::vector<float> m_cleaned_betaVals; // Local vector for cleaned up inputs
      bool m_doDichroic;

};

#endif
