// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#ifndef XAOD_STANDALONE
#include "AthenaKernel/MetaCont.h"
CLASS_DEF( MetaCont<xAOD::TruthMetaDataContainer> , 1268016587 , 1 )
#include "xAODCore/BaseInfo.h"
SG_BASE( MetaCont<xAOD::TruthMetaDataContainer>, MetaContBase );
#endif // not XAOD_STANDALONE

#endif // XAODTRUTH_TRUTHMETADATACONTAINER_H
