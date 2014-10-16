// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRUTH_TRUTHPILEUPEVENTAUXCONTAINER_H
#define XAODTRUTH_TRUTHPILEUPEVENTAUXCONTAINER_H

#include "xAODTruth/versions/TruthPileupEventAuxContainer_v1.h"

namespace xAOD {
   /// Declare the latest version of the truth event auxiliary container
   typedef TruthPileupEventAuxContainer_v1 TruthPileupEventAuxContainer;
}

// Declare a CLID for the class
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TruthPileupEventAuxContainer, 1274365729, 1 )

#endif
