// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthVertexContainer.h 622193 2014-10-16 16:08:34Z krasznaa $
#ifndef XAODTRUTH_TRUTHVERTEXCONTAINER_H
#define XAODTRUTH_TRUTHVERTEXCONTAINER_H

// Local include(s):
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/versions/TruthVertexContainer_v1.h"

namespace xAOD {
   /// Declare the latest version of the truth vertex container
   typedef TruthVertexContainer_v1 TruthVertexContainer;
}

// Declare a CLID for the class for Athena:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TruthVertexContainer, 1239726567, 1 )

#endif // XAODTRUTH_TRUTHVERTEXCONTAINER_H
