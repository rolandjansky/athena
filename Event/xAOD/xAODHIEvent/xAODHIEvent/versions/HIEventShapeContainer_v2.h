// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: HIEventShapeContainer_v2.h 693494 2015-09-07 14:59:45Z krasznaa $
#ifndef XAODHIEVENT_HIEVENTSHAPECONTAINER_V2_H
#define XAODHIEVENT_HIEVENTSHAPECONTAINER_V2_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODHIEvent/versions/HIEventShape_v2.h"

namespace xAOD {

   typedef DataVector< HIEventShape_v2 > HIEventShapeContainer_v2;

} // namespace xAOD

#endif // XAODHIEVENT_HIEVENTSHAPECONTAINER_V2_H
