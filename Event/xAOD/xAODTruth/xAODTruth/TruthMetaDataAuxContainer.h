// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTRUTH_TRUTHMETADATAAUXCONTAINER_H
#define XAODTRUTH_TRUTHMETADATAAUXCONTAINER_H

// Local include(s):
#include "xAODTruth/versions/TruthMetaDataAuxContainer_v1.h"

namespace xAOD {
   /// Declare the latest version of the truth vertex auxiliary container
   typedef TruthMetaDataAuxContainer_v1 TruthMetaDataAuxContainer;
}

// Declare a CLID for the class
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TruthMetaDataAuxContainer, 1094306618, 1 )

#endif // XAODTRUTH_TRUTHMETADATAAUXCONTAINER_H
