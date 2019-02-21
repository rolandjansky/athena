/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_energycorrelatorgeneralizedtool_header
#define jetsubstructuremomenttools_energycorrelatorgeneralizedtool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"

class EnergyCorrelatorGeneralizedTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(EnergyCorrelatorGeneralizedTool, IJetModifier)
    
    public:
      // Constructor and destructor
      EnergyCorrelatorGeneralizedTool(std::string name);

      int modifyJet(xAOD::Jet &injet) const;

    private:
      float m_Beta;
};

#endif
