// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: HIEventShapeAuxContainer.h 693494 2015-09-07 14:59:45Z krasznaa $
#ifndef XAODHIEVENT_HIEVENTSHAPEAUXCONTAINER_H
#define XAODHIEVENT_HIEVENTSHAPEAUXCONTAINER_H

// Local include(s):
#include "xAODHIEvent/versions/HIEventShapeAuxContainer_v2.h"

namespace xAOD {
   /// Define the latest version of the auxiliary container
   typedef HIEventShapeAuxContainer_v2 HIEventShapeAuxContainer;
}

// Set up a CLID for the type:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::HIEventShapeAuxContainer, 1170531618, 1 )

#endif // XAODHIEVENT_HIEVENTSHAPEAUXCONTAINER_H
