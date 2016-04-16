/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_isubjetrecordertool_header
#define jetsubstructuremomenttools_isubjetrecordertool_header

#include "AsgTools/AsgTool.h"
#include <vector>
#include "xAODJet/Jet.h"

namespace fastjet {
    class PseudoJet;
}

class ISubjetRecorderTool :
  virtual public asg::IAsgTool {
    ASG_TOOL_INTERFACE(ISubjetRecorderTool)

    public:
      virtual std::vector<xAOD::Jet *> recordSubjets(std::vector<fastjet::PseudoJet> subjets, xAOD::Jet &jet) const = 0;
};


#endif
