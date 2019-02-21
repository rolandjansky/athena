/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_nsubjetinesstool_header
#define jetsubstructuremomenttools_nsubjetinesstool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"

class NSubjettinessTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(NSubjettinessTool, IJetModifier)

    public:
      // Constructor and destructor
      NSubjettinessTool(std::string name);

      int modifyJet(xAOD::Jet &injet) const;

    private:
      float m_Alpha;
      
};

#endif
