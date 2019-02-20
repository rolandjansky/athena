/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_energycorrelatortool_header
#define jetsubstructuremomenttools_energycorrelatortool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"

class EnergyCorrelatorTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(EnergyCorrelatorTool, IJetModifier)
    
    public:
      // Constructor and destructor
      EnergyCorrelatorTool(std::string name);

      int modifyJet(xAOD::Jet &injet) const;

    private:
      float m_Beta;
      bool m_includeBeta2;
      bool m_includeECF4;
      
};

#endif
