/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_energycorrelatorgeneralizedtool_header
#define jetsubstructuremomenttools_energycorrelatorgeneralizedtool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"
#include "JetSubStructureMomentTools/ECFHelper.h"

class EnergyCorrelatorGeneralizedTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(EnergyCorrelatorGeneralizedTool, IJetModifier)
    
    public:
      // Constructor and destructor
      EnergyCorrelatorGeneralizedTool(std::string name);

      StatusCode initialize();

      int modifyJet(xAOD::Jet &injet) const;

    private:
      float m_Beta;
      bool m_doN3;
      bool m_doLSeries;
      std::vector<float> m_betaVals;
      std::vector<float> betaVals; // Local vector for cleaned up inputs

};

#endif
