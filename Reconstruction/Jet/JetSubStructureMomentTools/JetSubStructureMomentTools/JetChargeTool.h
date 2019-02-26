/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetChargeTool.h

#ifndef jetsubstructuremomenttools_JetChargeTool_H
#define jetsubstructuremomenttools_JetChargeTool_H

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"

class JetChargeTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(JetChargeTool, IJetModifier)

    public:
      // Constructor and destructor
      JetChargeTool(std::string name);

      int modifyJet(xAOD::Jet &jet) const;

    private:
      double m_k;

};

#endif
