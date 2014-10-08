/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include "xAODCaloEvent/CaloClusterContainer.h"


namespace jet{
  class LabelIndex;
}

namespace fastjet{
  class PseudoJet;
}

using jet::LabelIndex;
using xAOD::CaloCluster;
using fastjet::PseudoJet;

class ClusterToPseudoJet {
public:

  /* No special assigment operator for copy constructor, or destructor
   as the LableIndex is deleted by the caller */

  ClusterToPseudoJet(const LabelIndex* index_map);

  PseudoJet operator() (const CaloCluster* cluster);

private:

  const LabelIndex* m_indexMap;
};

