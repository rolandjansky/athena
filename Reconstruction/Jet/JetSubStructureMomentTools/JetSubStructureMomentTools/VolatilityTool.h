/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_volatilitytool_header
#define jetsubstructuremomenttools_volatilitytool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"

class VolatilityTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(VolatilityTool, IJetModifier)
    
    public:
      // Constructor and destructor
      VolatilityTool(std::string name);

      int modifyJet(xAOD::Jet &jet) const;

    private:
      unsigned int m_num_iterations;
      double m_zcut, m_dcut_fctr, m_exp_min, m_exp_max, m_rigidity, m_truncation_fctr;
};


#endif
