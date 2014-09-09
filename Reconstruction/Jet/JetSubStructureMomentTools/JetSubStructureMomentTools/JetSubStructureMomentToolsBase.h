/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_jetsubstructurebase_header
#define jetsubstructuremomenttools_jetsubstructurebase_header

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include "JetRec/JetModifierBase.h"
#include <vector>

using namespace std;

namespace fastjet {
  class PseudoJet;
}

class JetSubStructureMomentToolsBase :
  public JetModifierBase {
    public:
      // Constructor and destructor
      JetSubStructureMomentToolsBase(std::string name);
};


#endif
