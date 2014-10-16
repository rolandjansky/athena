// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthPileupEventContainer.h 622193 2014-10-16 16:08:34Z krasznaa $
#ifndef XAODTRUTH_TRUTHPILEUPEVENTCONTAINER_H
#define XAODTRUTH_TRUTHPILEUPEVENTCONTAINER_H

// Local include(s):
#include "xAODTruth/TruthPileupEvent.h"
#include "xAODTruth/versions/TruthPileupEventContainer_v1.h"

namespace xAOD {
   /// Declare the latest version of the truth event container
   typedef TruthPileupEventContainer_v1 TruthPileupEventContainer;
}

// Declare a CLID for the class for Athena:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TruthPileupEventContainer, 1141476193, 1 )

#endif // XAODTRUTH_TRUTHPILEUPEVENTCONTAINER_H
