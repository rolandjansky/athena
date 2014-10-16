// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthEventBaseContainer.h 622193 2014-10-16 16:08:34Z krasznaa $
#ifndef XAODTRUTH_TRUTHEVENTBASECONTAINER_H
#define XAODTRUTH_TRUTHEVENTBASECONTAINER_H

// Local include(s):
#include "xAODTruth/TruthEventBase.h"
#include "xAODTruth/versions/TruthEventBaseContainer_v1.h"

namespace xAOD {
   /// Declare the latest version of the truth event container
   typedef TruthEventBaseContainer_v1 TruthEventBaseContainer;
}

// Declare a CLID for the class for Athena:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TruthEventBaseContainer, 1280408222, 1 )

#endif // XAODTRUTH_TRUTHEVENTBASECONTAINER_H
