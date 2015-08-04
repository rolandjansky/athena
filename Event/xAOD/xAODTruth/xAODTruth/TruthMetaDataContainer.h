// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTRUTH_TRUTHMETADATACONTAINER_H
#define XAODTRUTH_TRUTHMETADATACONTAINER_H

// Local include(s):
#include "xAODTruth/TruthMetaData.h"
#include "xAODTruth/versions/TruthMetaDataContainer_v1.h"

namespace xAOD {
   /// Declare the latest version of the truth vertex container
   typedef TruthMetaDataContainer_v1 TruthMetaDataContainer;
}

// Declare a CLID for the class for Athena:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TruthMetaDataContainer, 1188015687, 1 )

#endif // XAODTRUTH_TRUTHMETADATACONTAINER_H
