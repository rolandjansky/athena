/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_boostedxbbtagtool_header
#define jetsubstructuremomenttools_boostedxbbtagtool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"

class BoostedXbbTagTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(BoostedXbbTagTool, IJetModifier)

    public:
      // Constructor and destructor
      BoostedXbbTagTool(std::string name);

      int modifyJet(xAOD::Jet &jet) const;

    private:
      std::string m_working_point,
                  m_recommendations_file,
                  m_boson_type,
                  m_algorithm_name,
                  m_muon_container_name;
      int m_num_bTags;
      std::string m_decor_prefix;
      bool m_debug,
           m_verbose;
};


#endif
