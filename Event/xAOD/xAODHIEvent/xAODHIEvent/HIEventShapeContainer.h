// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: HIEventShapeContainer.h 693427 2015-09-07 11:45:19Z krasznaa $
#ifndef XAODHIEVENT_HIEVENTSHAPECONTAINER_H
#define XAODHIEVENT_HIEVENTSHAPECONTAINER_H

// Local include(s):
#include "xAODHIEvent/versions/HIEventShapeContainer_v1.h"
#include "xAODHIEvent/HIEventShape.h"

namespace xAOD {
   /// Define the latest version of the container
   typedef HIEventShapeContainer_v1 HIEventShapeContainer;
}

// Set up a CLID for the type:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::HIEventShapeContainer, 1155025655, 1 )

#endif // XAODHIEVENT_HIEVENTSHAPECONTAINER_H
