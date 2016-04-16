/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_bosontagtool_header
#define jetsubstructuremomenttools_bosontagtool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"

class BosonTagTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(BosonTagTool, IJetModifier)

    public:
      // Constructor and destructor
      BosonTagTool(std::string name);

      int modifyJet(xAOD::Jet &jet) const;

    private:
      std::string m_working_point,
                  m_tagger_alg,
                  m_recommendations_file;
      bool m_debug,
           m_verbose;
};


#endif
