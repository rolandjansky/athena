// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODEGAMMA_VERSIONS_ELECTRONCONTAINER_V1_H
#define XAODEGAMMA_VERSIONS_ELECTRONCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODEgamma/versions/Electron_v1.h"
#include "xAODEgamma/versions/EgammaContainer_v1.h"


namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::Electron_v1 > ElectronContainer_v1;
}

#endif // XAODEGAMMA_VERSIONS_ELECTRONCONTAINER_V1_H
