// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthVertexAuxContainer.h 622081 2014-10-16 00:55:18Z abuckley $
#ifndef XAODTRUTH_TRUTHVERTEXAUXCONTAINER_H
#define XAODTRUTH_TRUTHVERTEXAUXCONTAINER_H

// Local include(s):
#include "xAODTruth/versions/TruthVertexAuxContainer_v1.h"

namespace xAOD {
   /// Declare the latest version of the truth vertex auxiliary container
   typedef TruthVertexAuxContainer_v1 TruthVertexAuxContainer;
}

// Declare a CLID for the class
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TruthVertexAuxContainer, 1254939514, 1 )

#endif // XAODTRUTH_TRUTHVERTEXAUXCONTAINER_H
