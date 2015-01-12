/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_nsubjetinessratiostool_header
#define jetsubstructuremomenttools_nsubjetinessratiostool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"

class NSubjettinessRatiosTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(NSubjettinessRatiosTool, IJetModifier)

    public:
      // Constructor and destructor
      NSubjettinessRatiosTool(std::string name);

      int modifyJet(xAOD::Jet &jet) const;
};


#endif
