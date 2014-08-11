// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTRIGGER_VERSIONS_TRIGCOMPOSITECONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_TRIGCOMPOSITECONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigger/versions/TrigComposite_v1.h"

namespace xAOD {
   /// The container is a simple typedef
   typedef DataVector< TrigComposite_v1 > TrigCompositeContainer_v1;
}

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigCompositeContainer_v1, 1333228823, 1 )
#endif // not XAOD_STANDALONE

#endif // XAODTRIGGER_VERSIONS_TRIGCOMPOSITECONTAINER_V1_H
