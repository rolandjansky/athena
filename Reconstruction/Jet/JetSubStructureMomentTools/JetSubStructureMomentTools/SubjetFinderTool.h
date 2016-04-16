/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_subjetfindertool_header
#define jetsubstructuremomenttools_subjetfindertool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"
#include "JetSubStructureMomentTools/ISubjetRecorderTool.h"
#include "AsgTools/ToolHandle.h"

class SubjetFinderTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(SubjetFinderTool, IJetModifier)

    public:
      // Constructor and destructor
      SubjetFinderTool(std::string name);

      int modifyJet(xAOD::Jet &jet) const;

    private:
      std::string m_jetalg;
      float m_jetrad;
      float m_ptmin;
      int m_exclusivenjets;
      ToolHandle<ISubjetRecorderTool> m_subjetrecorder;
};


#endif
