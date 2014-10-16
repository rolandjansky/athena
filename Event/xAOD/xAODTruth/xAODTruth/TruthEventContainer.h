// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthEventContainer.h 622193 2014-10-16 16:08:34Z krasznaa $
#ifndef XAODTRUTH_TRUTHEVENTCONTAINER_H
#define XAODTRUTH_TRUTHEVENTCONTAINER_H

// Local include(s):
#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventBaseContainer.h"
#include "xAODTruth/versions/TruthEventContainer_v1.h"

namespace xAOD {
   /// Declare the latest version of the truth event container
   typedef TruthEventContainer_v1 TruthEventContainer;
}

// Declare a CLID for the class for Athena:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TruthEventContainer, 1200227006, 1 )

#endif // XAODTRUTH_TRUTHEVENTCONTAINER_H
