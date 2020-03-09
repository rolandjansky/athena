/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_multiplicitiestool_header
#define jetsubstructuremomenttools_multiplicitiestool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"

class MultiplicitiesTool :
public JetSubStructureMomentToolsBase {
  ASG_TOOL_CLASS(MultiplicitiesTool, IJetModifier)
    
    public:
      // Constructor and destructor
      MultiplicitiesTool(std::string name);
      
      virtual int modifyJet(xAOD::Jet &injet) const override;
};

#endif
