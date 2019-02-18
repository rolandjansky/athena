/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_energycorrelatorgeneralizedratiostool_header
#define jetsubstructuremomenttools_energycorrelatorgeneralizedratiostool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"

class EnergyCorrelatorGeneralizedRatiosTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(EnergyCorrelatorGeneralizedRatiosTool, IJetModifier)
    
    public:
      // Constructor and destructor
      EnergyCorrelatorGeneralizedRatiosTool(std::string name);

      int modifyJet(xAOD::Jet &jet) const;

    private:
};

#endif
