/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetPullTool.h

#ifndef jetsubstructuremomenttools_pulltool_header
#define jetsubstructuremomenttools_pulltool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"

class JetPullTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(JetPullTool, IJetModifier)

    public:
      // Constructor and destructor
      JetPullTool(std::string name);

      int modifyJet(xAOD::Jet &injet) const;

    private:
      bool m_useEtaInsteadOfY;
      bool m_includeTensorMoments;

};

#endif
