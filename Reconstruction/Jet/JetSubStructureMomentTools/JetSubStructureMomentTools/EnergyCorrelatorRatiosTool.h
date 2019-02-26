/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_energycorrelatorratiostool_header
#define jetsubstructuremomenttools_energycorrelatorratiostool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"

class EnergyCorrelatorRatiosTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(EnergyCorrelatorRatiosTool, IJetModifier)
    
    public:
      // Constructor and destructor
      EnergyCorrelatorRatiosTool(std::string name);

      int modifyJet(xAOD::Jet &jet) const;

    private:
      bool m_includeBeta2;
      bool m_includeECF4;
            
};

#endif
