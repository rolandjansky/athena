// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthEventAuxContainer.h 622193 2014-10-16 16:08:34Z krasznaa $
#ifndef XAODTRUTH_TRUTHEVENTAUXCONTAINER_H
#define XAODTRUTH_TRUTHEVENTAUXCONTAINER_H

// Local include(s):
#include "xAODTruth/versions/TruthEventAuxContainer_v1.h"

namespace xAOD {
   /// Declare the latest version of the truth event auxiliary container
   typedef TruthEventAuxContainer_v1 TruthEventAuxContainer;
}

// Declare a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TruthEventAuxContainer, 1274265725, 1 )

#endif // XAODTRUTH_TRUTHEVENTAUXCONTAINER_H
