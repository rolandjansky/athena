// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: HIEventShapeContainer.h 594315 2014-04-25 17:34:40Z krasznaa $
#ifndef XAODHIEVENT_HIEVENTSHAPECONTAINER_H
#define XAODHIEVENT_HIEVENTSHAPECONTAINER_H

// Local include(s):
#include "xAODHIEvent/versions/HIEventShapeContainer_v1.h"
#include "xAODHIEvent/HIEventShape.h"

namespace xAOD {
   /// Define the latest version of the container
   typedef HIEventShapeContainer_v1 HIEventShapeContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::HIEventShapeContainer, 1219812540, 1)

#endif // XAODEVENTINFO_EVENTINFOCONTAINER_H
