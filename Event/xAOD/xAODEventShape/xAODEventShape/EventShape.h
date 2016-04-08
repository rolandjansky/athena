// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventShape.h 629489 2014-11-19 14:37:55Z krasznaa $
#ifndef XAODEVENTSHAPE_EVENTSHAPE_H
#define XAODEVENTSHAPE_EVENTSHAPE_H

// Local include(s):
#include "xAODEventShape/versions/EventShape_v1.h"

namespace xAOD {
   /// Definition of the current event format version
   typedef EventShape_v1 EventShape;
}

// Specify a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::EventShape, 242958214, 1 )

#endif // XAODEVENTSHAPE_EVENTSHAPE_H
