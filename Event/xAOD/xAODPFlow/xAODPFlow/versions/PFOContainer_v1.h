// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PFOContainer_v1.h 630853 2014-11-25 11:15:03Z mhodgkin $
#ifndef XAODPFLOW_VERSIONS_PFOCONTAINER_V1_H
#define XAODPFLOW_VERSIONS_PFOCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODPFlow/versions/PFO_v1.h"

namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::PFO_v1 > PFOContainer_v1;
}

#endif // XAODPFLOW_VERSIONS_PFOCONTAINER_V1_H
