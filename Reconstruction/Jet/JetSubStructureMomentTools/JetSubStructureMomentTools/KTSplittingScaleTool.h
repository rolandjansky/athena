/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_ktsplittingscaletool_header
#define jetsubstructuremomenttools_ktsplittingscaletool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"

class KTSplittingScaleTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(KTSplittingScaleTool, IJetModifier)
    
    public:
      // Constructor and destructor
      KTSplittingScaleTool(std::string name);

      int modifyJet(xAOD::Jet &injet) const;
};

#endif
