/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETREC_CLUSTERTOPSEUDOJET_H
#define TRIGHLTJETREC_CLUSTERTOPSEUDOJET_H

#include <string>
#include "xAODCaloEvent/CaloClusterContainer.h"
/*Function object to convdert CaloClusters to fastjet::Pseudojets
in order that they can be used as input to the fast jet algorithms.
The pseudojets keep track of the clusters, to allow detector
specific caculation (eg jet cleaning) once the jets have been found */


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
#endif
