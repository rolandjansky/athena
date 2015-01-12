/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_subjetmakertool_header
#define jetsubstructuremomenttools_subjetmakertool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"

#include <string>

class SubjetMakerTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(SubjetMakerTool, IJetModifier)
    
    public:
      // Constructor and destructor
      SubjetMakerTool(std::string name);

      std::string m_type;
      float m_R;
      float m_minPt;

      int modifyJet(xAOD::Jet &jet) const;
};


#endif
