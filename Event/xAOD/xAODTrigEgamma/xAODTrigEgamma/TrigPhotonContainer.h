// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigPhotonContainer.h 631414 2014-11-26 22:03:16Z gwatts $
#ifndef XAODTRIGEGAMMA_TRIGPHOTONCONTAINER_H
#define XAODTRIGEGAMMA_TRIGPHOTONCONTAINER_H

// Local include(s):
#include "xAODTrigEgamma/TrigPhoton.h"
#include "xAODTrigEgamma/versions/TrigPhotonContainer_v1.h"

namespace xAOD {
   /// Declare the latest version of the container
   typedef TrigPhotonContainer_v1 TrigPhotonContainer;
}

#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigPhotonContainer, 1141180511, 1 )

#endif // not XAODTRIGEGAMMA_TRIGPHOTONCONTAINER_H
