/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_energycorrelatortool_header
#define jetsubstructuremomenttools_energycorrelatortool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"
#include "JetSubStructureMomentTools/ECFHelper.h"

class EnergyCorrelatorTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(EnergyCorrelatorTool, IJetModifier)
    
    public:
      // Constructor and destructor
      EnergyCorrelatorTool(std::string name);
     
      StatusCode initialize();

      int modifyJet(xAOD::Jet &injet) const;

    private:
      float m_Beta;
      bool m_includeECF4;
      std::vector<float> m_betaVals;
      std::vector<float> betaVals; // Local vector for cleaned up inputs

};

#endif
