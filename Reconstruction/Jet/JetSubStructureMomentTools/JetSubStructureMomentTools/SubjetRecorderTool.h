/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_subjetrecordertool_header
#define jetsubstructuremomenttools_subjetrecordertool_header

#include "AsgTools/AsgTool.h"
#include "xAODJet/Jet.h"
#include "JetSubStructureMomentTools/ISubjetRecorderTool.h"

namespace fastjet {
    class PseudoJet;
}

class SubjetRecorderTool :
  public asg::AsgTool, 
  virtual public ISubjetRecorderTool {
    ASG_TOOL_CLASS(SubjetRecorderTool, ISubjetRecorderTool)

    public:
      // Constructor and destructor
      SubjetRecorderTool(std::string name);

      std::vector<xAOD::Jet *> recordSubjets(std::vector<fastjet::PseudoJet> subjets, xAOD::Jet &jet) const;

    private:
      std::string m_subjetlabel;
      std::string m_subjetcontainername;
};


#endif
