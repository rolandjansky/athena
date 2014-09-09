/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_centerofmassshapestool_header
#define jetsubstructuremomenttools_centerofmassshapestool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"

class CenterOfMassShapesTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(CenterOfMassShapesTool, IJetModifier)
    
    public:
      // Constructor and destructor
      CenterOfMassShapesTool(std::string name);

      int modifyJet(xAOD::Jet &jet) const;
};


#endif
