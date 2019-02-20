/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_dipolaritytool_header
#define jetsubstructuremomenttools_dipolaritytool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"

class DipolarityTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(DipolarityTool, IJetModifier)
    
    public:
      // Constructor and destructor
      DipolarityTool(std::string name);

      int modifyJet(xAOD::Jet &injet) const;

    private:
      double m_SubJetRadius;
};

#endif
