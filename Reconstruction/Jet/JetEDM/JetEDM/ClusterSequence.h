/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ClusterSequence.h

#ifndef jet__ClusterSequence_H
#define jet__ClusterSequence_H

#include <vector>
#include "fastjet/ClusterSequence.hh"
#include "xAODCore/CLASS_DEF.h"

// David Adams
// January 2014
//
// Container of fastjet cluster sequences.

namespace jet {

  typedef fastjet::ClusterSequence ClusterSequence;

}  // end jet namespace

CLASS_DEF( fastjet::ClusterSequence , 38719120 , 1 )

#endif
