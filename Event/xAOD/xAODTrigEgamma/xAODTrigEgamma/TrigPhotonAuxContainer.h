// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigPhotonAuxContainer.h 631414 2014-11-26 22:03:16Z gwatts $
#ifndef XAODTRIGEGAMMA_TRIGPHOTONAUXCONTAINER_H
#define XAODTRIGEGAMMA_TRIGPHOTONAUXCONTAINER_H

// Local include(s):
#include "xAODTrigEgamma/versions/TrigPhotonAuxContainer_v1.h"

namespace xAOD {
   /// Declare the latest version of the photon auxiliary container
   typedef TrigPhotonAuxContainer_v1 TrigPhotonAuxContainer;
}

#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigPhotonAuxContainer, 1313537382, 1 )

#endif // not XAODTRIGEGAMMA_TRIGPHOTONAUXCONTAINER_H
