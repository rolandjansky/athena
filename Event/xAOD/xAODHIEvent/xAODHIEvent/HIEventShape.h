// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: HIEventShape.h 693494 2015-09-07 14:59:45Z krasznaa $
#ifndef XAODHIEVENT_HIEVENTSHAPE_H
#define XAODHIEVENT_HIEVENTSHAPE_H

// Local include(s):
#include "xAODHIEvent/versions/HIEventShape_v2.h"

namespace xAOD {
   /// Definition of the latest event info version
   typedef HIEventShape_v2 HIEventShape;
}

// Set up a CLID for the type:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::HIEventShape, 203448259, 1 )

#endif // XAODHIEVENT_HIEVENTSHAPE_H
